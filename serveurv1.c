/**
* \file serveur.c
* \brief Fichier d'implementation de la classe serveur
* \date 13/03/2015
* Commande :
* ./serveur.exe
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "serveur.h"

/**
* \brief Initialisation de la connexion
* \return Socket du serveur
*/
int init_connexion(void){

	//Initialisation de la socket du serveur
	sockaddr_in sin = { 0 };

	if((serveur = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		perror("socket()");
		exit(-1);
	}

	//Ouvert sur le port PORT_SERVEUR a n'importe qui
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	sin.sin_port = htons(PORT_SERVEUR);
	sin.sin_family = AF_INET;

	if(bind(serveur,(sockaddr *) &sin, sizeof sin) == -1){
		perror("bind()");
		exit(-1);
	}

	if(listen(serveur, MAX_CLIENTS) == -1){
		perror("listen()");
		exit(-1);
	}
	
	//Création de la grille de jeu
    srand(time(NULL));
    init(&g);
    printGrid(g);
	
	printf("Numero de port pour la connexion au serveur : %d \n", ntohs(sin.sin_port));  
    printf("Lancement de l'écoute\n");

	return serveur;
}

/**
* \brief Methode du thread de prise en charge d'un nouveau client
* \param[in] socket_desc Socket du nouveau client
*/
void *prise_en_charge_client(void *socket_desc){

	int sock = *(int*)socket_desc;
	int read_size;
	char message[2000], client_message[2000];
	char *m = "\nChoisissez des coordonnées d'attaque: \n";
	
    ResponseGet res1;
    //Message d'arrivée sur le serveur
    strcat(res1.msg,"\n*\nBienvenue sur le serveur de jeu de In-The-Navy.\n Tentez de couler les bateaux de la grille avant les autres joueurs.\n*\n\n");
	getOponentGrid(g, res1.grid);
	int length = sizeof(ResponseGet)+sizeof(char);
	//Envoi du message de bienvenue au client
	envoi_trame(sock, serializeResponseGet(res1), length);

	//En attente de reception de message
    while((read_size = recv(sock , client_message, 2000 , 0)) > 0){
        if(client_message[0] == '1'){			//code pour message d'attaque
        	
        	//Traitement des coordonnees d'attaque
			PositionLetterDigit p;
		    p.letter = client_message[1];
		    char subbuff[3];
		    memcpy( subbuff, &client_message[2], 2 );
		    subbuff[2] = '\0';
		    p.y = atoi(subbuff);
		    
			ResponseAttack res;
			res.result = attack(&g, p);
			memcpy(&res.pos, client_message+sizeof(char), 3*sizeof(char));

			sockaddr_in addr;
			socklen_t addr_size = sizeof(sockaddr_in);
			int err = getpeername(sock, (sockaddr *)&addr, &addr_size);
			res.who = addr.sin_addr;
    
			getOponentGrid(g, res.grid);
			printGrid(g);
			
			length = sizeof(ResponseAttack)+sizeof(char);
			diffusion(serializeResponseAttack(res), length);
			memset(client_message, 0, sizeof(client_message));

			//Si le dernier bateau est coule, une nouvelle grille est generee et une nouvelle partie commence
		    if (res.result == WIN){
		    	length = sizeof(ResponseGet)+sizeof(char);
		        init(&g);
		        ResponseGet resWin;
		        getOponentGrid(g, resWin.grid);
		        strcat(resWin.msg, "\nPartie terminée ! Tous les bateaux ont été coulé.\nChangement de grille...\n");
		        printGrid(g);
		        diffusion(serializeResponseGet(resWin), length);
		    }
		}
    }
    
    //Deconnexion d'un client
    if(read_size == 0){
        printf("Le client %d s'est déconnecté.\n", (sock-3));
        fflush(stdout);
        suppr_client(sock);
    }else if(read_size == -1){
        perror("recv()");
    }

    free(socket_desc);
     
    return 0;
}

/**
* \brief Envoi d'une trame a un client donne
* \param[in] sock Socket du client
* \param[in] msg Message envoye au client
* \param[in] length Longueur du message
*/
void envoi_trame(int sock, char* msg, int length){

	int offset = 0;
	//Segmentation du message en trames de taille maximale fixee
    while (offset < length){
        Trame t;
        t.idTrame = idTrame;
        t.index = offset;
        t.taille = length;
        memcpy(t.data, msg+offset, TAILLE_MAX_DATA_TRAME);
        offset+= TAILLE_MAX_DATA_TRAME;
        char *str_trame = serializeTrame(t);

		//Envoi de chaque trame a chaque client
        if((send(sock, str_trame, TAILLE_MAX_TRAME, 0)) < 0){
        	perror("send()");
        	exit(-1);
        }
    }
    idTrame++;
}

/**
* \brief Ajout d'un nouveau client a la liste
* \param[in] sock Le socket du client
*/
void ajout_client(int sock){

	Liste_clients *nouveau = malloc(sizeof(Liste_clients));
	nouveau->socket = sock;
	nouveau->next = NULL;
		
	if(clients == NULL){
		clients = nouveau;
	}else{
		Liste_clients *tmp = clients;
		while(tmp->next != NULL){
			tmp = tmp->next;
		}		
		tmp->next = nouveau;
	}
}

/**
* \brief Methode de suppression d'un client de la liste de diffusion suite a une deconnexion
* \param[in] sock Socket du client a oublier
*/
void suppr_client(int sock){

	if (clients){
        if (clients->socket == sock){
            Liste_clients *tmp = clients;
            clients = clients->next;
            close(tmp->socket);
            free(tmp);
        }else{

            Liste_clients *it = clients;
    
            while(it->next && clients->next->socket != sock){
                it = it->next;
            }

            if (it->next){
                Liste_clients *tmp = it->next;
                it->next = it->next->next;
                close(tmp->socket);
                free(tmp);
            }else{
                perror("erreur : ce client n'existe pas.");
            }
        }
    }
}

/**
* \brief Methode de vidage de la liste de clients
*/
void vide_clients(){

	Liste_clients *client_courant = clients;
	while (client_courant !=NULL){
		close(client_courant->socket);
		client_courant = client_courant->next;
	}
}

/**
* \brief Demande une position d'attaque au client et l'envoie au serveur
* \param[in] str Message a diffuser aux clients
* \param[in] length Taille du message a diffuser
*/
void diffusion(char* str, int length){

	int offset = 0;
    while (offset < length){
    	//Segmentation du message en trames de taille maximale fixee
        Trame t;
        t.idTrame = idTrame;
        t.index = offset;
        t.taille = length;
        memcpy(t.data, str+offset, TAILLE_MAX_DATA_TRAME);
        offset+= TAILLE_MAX_DATA_TRAME;
        int taille_trame = TAILLE_MAX_DATA_TRAME + 3*sizeof(int);
        
        //Envoi de chaque trame a chaque client
		Liste_clients *client_courant = clients;
		while (client_courant !=NULL){

			if(send(client_courant->socket, t.data, taille_trame, 0) < 0){
				perror("send()");
				continue;
			}
			client_courant = client_courant->next;
		}
    }
    idTrame++;
}

/**
* \brief Methode appelee lorsque le programme serveur se termine
*/
void byebye(void){

	char buffer[100] = "-\nLe serveur est hors-ligne.\n-";
	printf("%s\n",buffer);
	
	//Envoi a tous les clients du message de fermeture du serveur
	diffusion(buffer, 100);
	
	//Destructions
	vide_clients();
	
	//Fermeture du socket
	close(serveur);

}

/**
* \brief Redirection d'une fermeture de programme par la commande "Ctrl+c" vers la methode "byebye"
* \param[in] e
*/
void ctrlC_Handler(int e){
    exit(0);
}

/**
* \brief Main, creation de la connexion
* \param[in] argc
* \param[in] *argv
* \return exit(0)
*/
int main(int argc, char **argv){

	atexit(byebye);
    signal(SIGINT, ctrlC_Handler);
    
    //Initialisation de la connexion sur le serveur, attribution des ports
	int sock = init_connexion();

	int nouv_client, *csock;
	sockaddr_in adr_client;
	char *message;
	int c = sizeof(sockaddr_in);
	
	//En attente de nouveaux clients a heberger
    while((nouv_client = accept(sock, (sockaddr *)&adr_client, (socklen_t*)&c)))
    {
        printf("Nouveau client hébergé.\n");
         
        pthread_t sniffer_thread;
        csock = malloc(1);
        *csock = nouv_client;
        ajout_client(nouv_client);
        
        //Creation du thread de prise en charge de ce client
        if(pthread_create(&sniffer_thread, NULL , prise_en_charge_client, (void*)csock) < 0){
            perror("pthread_create()");
            return -1;
        }
    }
     
    if (nouv_client<0){
        perror("accept()");
        return -1;
    }

	return 0;
}

