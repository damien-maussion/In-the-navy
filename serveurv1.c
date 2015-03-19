#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "serveur.h"
//#include "client.h"

static int app(void){

	int sock = init_connection();
	char buffer[BUF_SIZE];

	int nouv_client, *csock;
	struct sockaddr_in client;
	char *message;

	int c = sizeof(struct sockaddr_in);
    while((nouv_client = accept(sock, (struct sockaddr *)&client, (socklen_t*)&c)))
    {
        printf("Nouveau client hébergé.\n");
         
        pthread_t sniffer_thread;
        csock = malloc(1);
        *csock = nouv_client;
        ajout_client(nouv_client);
         
        if(pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) csock) < 0){
            perror("could not create thread");
            return -1;
        }
         
        //Now join the thread , so that we dont terminate before the thread
        //pthread_join( sniffer_thread , NULL);
        //printf("Handler assigned\n");
    }
     
    if (nouv_client<0){
        perror("accept failed");
        return -1;
    }
}

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

void envoiTrame(int sock, char* msg){
	int offset = 0;
	int length = strlen(msg);
    while (offset < length){
        Trame t;
        t.idTrame = idTrame;
        t.index = offset;
        t.taille = length;
        memcpy(t.data, msg+offset, TAILLE_MAX_DATA_TRAME);
        offset+= TAILLE_MAX_DATA_TRAME;
        char *str_trame = serializeTrame(t);
    	int length_trame = TAILLE_MAX_DATA_TRAME + 3*sizeof(int);

        if((send(sock, str_trame, length_trame, 0)) < 0){
        	perror("send()");
        	exit(-1);
        }
    }
    idTrame++;
}

void *connection_handler(void *socket_desc){

	int sock = *(int*)socket_desc;
	int read_size;
	char message[2000], client_message[2000];
	char *m = "\nChoisissez des coordonnées d'attaque: \n";
    
    //Message d'arrivée sur le serveur
    char *bienvenue = "*\nBienvenue sur le serveur de jeu de In-The-Navy.\n Tentez de couler les bateaux de la grille avant les autres joueurs.\n*\n";
    strcat(message, bienvenue);
    strcat(message, getGrid(g));
    strcat(message, m);
    //printf("%s\n",message);
	envoiTrame(sock, message);
     
    //Receive a message from client
    while((read_size = recv(sock , client_message , 2000 , 0)) > 0){
        if(client_message[0] == '1'){
			//printf("reception attaque + diff\n");
			PositionLetterDigit p;
		    p.letter = client_message[1];
		    char subbuff[3];
		    memcpy( subbuff, &client_message[2], 2 );
		    subbuff[2] = '\0';
		    p.y = atoi(subbuff);
		    
			ResponseAttack res;
			res.result = attack(&g, p);
			getOponentGrid(g, res.grid);
			printGrid(g);
			
			char n[20];
			sprintf(n, " par le client %d", sock);
			//diffusion(n);
			printf("%s\n",serializeResponseAttack(res));
			strcat(client_message, n);
			strcat(client_message, getGrid(g));
			strcat(client_message, m);
			diffusion(client_message);
			memset(client_message, 0, sizeof(client_message));

		    if (res.result == WIN){
		        init(&g);
		        ResponseGet res;
		        getOponentGrid(g, res.grid);
		        printf("\nGrille finie => Creation et envoi d'une nouvelle grille.\n");
		        printGrid(g);
		        diffusion(serializeResponseGet(res));
		    }
		}
    }
     
    if(read_size == 0){
        printf("Le client %d s'est déconnecté.\n", (sock-3));
        fflush(stdout);
        remove_client(sock);
    }else if(read_size == -1){
        perror("recv()");
    }

    free(socket_desc);
     
    return 0;
}

void clear_clients(){

	Liste_clients *client_courant = clients;
	while (client_courant !=NULL){
		close(client_courant->socket);
		client_courant = client_courant->next;
	}
}

static void remove_client(int sock){

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

/*static void send_message_to_all_clients(Client *clients, Client sender, int actual, const char *buffer, char from_server){

	int i = 0;
	char message[BUF_SIZE];
	message[0] = 0;
	for(i = 0; i < actual; i++){
		//we don't send message to the sender
		if(sender.sock != clients[i].sock){
			if(from_server == 0)
			{
				strncpy(message, sender.name, BUF_SIZE - 1);
				strncat(message, " : ", sizeof message - strlen(message) - 1);
			}
			strncat(message, buffer, sizeof message - strlen(message) - 1);
			write_client(clients[i].sock, message);
		}
	}
}*/

static int init_connection(void){

	//Initialisation de la socket du serveur
	sockaddr_in sin = { 0 };

	if((serveur = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		perror("socket()");
		exit(-1);
	}

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
	
	//Création de la grille
    srand(time(NULL));
    init(&g);
    printGrid(g);

	//Initialisation du mutex
	/*if (pthread_mutex_init(&mutex_grid, NULL) != 0)
    {
        perror("\n mutex_grid init failed\n");
        return 1;
    }*/
	
	printf("Numero de port pour la connexion au serveur : %d \n", ntohs(sin.sin_port));  
    printf("Lancement de l'écoute\n");

	return serveur;
}

static void end_connection(int sock){

   close(sock);
}

static int read_client(int sock, char *buffer){

	/*int n = 0;

	if((n = recv(sock, buffer, BUF_SIZE - 1, 0)) < 0){
		perror("recv()");
		// if recv error we disonnect the client
		n = 0;
	}

	buffer[n] = 0;

	return n;*/
	return 0;
}

static void write_client(int sock, const char *buffer){

	/*if(send(sock, buffer, strlen(buffer), 0) < 0){
		perror("send()");
		exit(-1);
	}*/
}

void diffusion(char* str){

	int offset = 0;
	int length = strlen(str)+4;
    while (offset < length){
        Trame t;
        t.idTrame = idTrame;
        t.index = offset;
        t.taille = length;
        memcpy(t.data, str+offset, TAILLE_MAX_DATA_TRAME);
        offset+= TAILLE_MAX_DATA_TRAME;
		Liste_clients *client_courant = clients;
		int taille_trame = TAILLE_MAX_DATA_TRAME + 3*sizeof(int);
		while (client_courant !=NULL){

			if(send(client_courant->socket, t.data, taille_trame, 0) < 0){
				perror("send()");
				continue;
			}
			client_courant = client_courant->next;
		}
    }
    idTrame++;
    //send(clients->socket, str, strlen(str), 0);
}

void byebye(void){

	char buffer[100] = "-\nLe serveur est hors-ligne.\n-";
	printf("%s\n",buffer);
	diffusion(buffer);
	clear_clients();
	end_connection(serveur);
}

void ctrlC_Handler(int e){
    exit(0);
}

int main(int argc, char **argv){

	atexit(byebye);
    signal(SIGINT, ctrlC_Handler);
    
	app();

	return 0;
}

