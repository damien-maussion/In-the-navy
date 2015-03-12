/*-----------------------------------------------------------
Client a lancer apres le serveur avec la commande :
client <adresse-serveur>
------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#include "client.h"

void lanceAttack(int sock){
	char pos[3];
	char buffer[4] = "1";

	printf ("Choisissez des coordonnées d'attaque: ");
	scanf ("%3s",pos);  
	
	strcat(buffer, pos);

	if(send(sock, buffer, strlen(buffer), 0) < 0){
		perror("send()");
		exit(-1);
	}else{
		printf("Grille de votre adversaire :\n %s\n", oponentGrid);
		printf("Votre grille :\n %s\n", getGrid(grid));
	}
	
	if(partieEnCours)
		attente(sock);
}

void attente(int csock){
	printf("Attendez que votre adversaire joue.\n");
	char buffer[256];
	int n = 0;
	if((n = recv(csock, buffer, sizeof buffer - 1, 0)) < 0){
		perror("recv()");
		exit(-1);
	}
	buffer[n] = '\0';
	if(buffer[0] == '-'){ 				//déconnexion
		printf("%s\n",buffer);
		printf("Partie terminée. Reconnexion avec le serveur de jeu en cours...\n");
		partieEnCours = false;
	}else if(buffer[0] == '1'){			//attaque
		printf("Grille de votre adversaire :\n %s\n", oponentGrid);
		printf("Votre grille :\n %s\n", getGrid(grid));
		printf("%s\n",buffer);
	}else{								//erreur
		perror("erreur : données incomprises");
	}
	
	if(partieEnCours)
		lanceAttack(csock);
}

void envoiGrille(){
	char* Buffer = getGrid(grid);
	if(send(adversaire, Buffer, 1024, 0) < 0){
		perror("send()");
		exit(-1);
	}
}

void receptionGrille(){
	if((recv(adversaire, oponentGrid, 1024, 0)) < 0){
		perror("recv()");
		exit(-1);
	}
		
}

/*void* prise_en_charge(void *args){
   //cast
    args_traitement *args_t = args;
    
    char buffer[256];
    int longueur;
   
    if ((longueur = read(args_t->soc, buffer, sizeof(buffer))) <= 0) 
        return NULL;
    
    char c= buffer[0];
    if (c=='0'){		
    	printf("%s",buffer);
    	printf("pwet\n");
    }else if (c=='1'){ 					//attaque
    	printf("attaque recue\n");
    	PositionLetterDigit p;
        p.letter = buffer[1];
        char subbuff[3];
        memcpy( subbuff, &buffer[2], 2 );
        subbuff[2] = '\0';
        p.y = atoi(subbuff);
        
        char res[10];
        res[0] = '0';
        resultAttack resA= attack(&grid, p);
        memcpy(res+1, &resA, sizeof(resultAttack));
        memcpy(res+sizeof(resultAttack), buffer+1, 3);

        write(args_t->soc, res, 10);
        lanceAttack(args_t->soc);    
        printf("attack\n");
    }
}*/

void byebye(void){
	printf("\nVous êtes désormais déconnecté(e).\n");
	if(send(adversaire, "-Votre adversaire a quitté la partie.\n", 50, 0) < 0)
	{
		perror("send()");
		exit(-1);
	}
}

void ctrlC_Handler(int e){
    exit(0);
}

int main(int argc, char **argv){
  
    int socket_descriptor; 		/* descripteur de socket */
	int longueur; 				/* longueur d'un buffer utilisé */
    hostent * hostinfo; 		/* info sur une machine hote */
    servent * ptr_service; 		/* info sur service */
    char buffer[2560];
    char * prog; 				/* nom du programme */
    char * host; 				/* nom de la machine distante */
  
  	atexit(byebye);
    signal(SIGINT, ctrlC_Handler);
      
    if (argc != 2) {
		perror("usage : client <adresse-serveur>");
		exit(1);
    }
   
    prog = argv[0];
    host = argv[1];
    
    printf("nom de l'executable : %s \n", prog);
    printf("adresse du serveur  : %s \n", host);
    
    if ((hostinfo = gethostbyname(host)) == NULL) {
		perror("erreur : impossible de trouver le serveur a partir de son adresse.");
		exit(1);
    }
    
    adresse_locale_globale.sin_addr = *(in_addr *) hostinfo->h_addr;
	adresse_locale_globale.sin_port = htons(5000);
	adresse_locale_globale.sin_family = AF_INET;

    printf("numero de port pour la connexion au serveur : %d \n", ntohs(adresse_locale_globale.sin_port));
    
    /* creation de la socket */
    if ((socket_descriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("erreur : impossible de creer la socket de connexion avec le serveur.");
		exit(1);
    }
    
    /* tentative de connexion au serveur dont les infos sont dans adresse_locale_globale */
    if ((connect(socket_descriptor, (sockaddr*)(&adresse_locale_globale), sizeof(adresse_locale_globale))) < 0) {
		perror("erreur : impossible de se connecter au serveur.");
		exit(1);
    }
    
    srand(time(NULL));
    init(&grid);
    
    printf("Connexion au serveur établie. \n");
    
    char oponent[50];
    int n = 0;
	if((n = recv(socket_descriptor, oponent, 49, 0)) < 0){
		perror("recv()");
		exit(-1);
	}
	oponent[n] = 0;

    printf("addr :: %s\n", oponent);
  	close(socket_descriptor);
    
    if (strcmp(oponent, "fake")==0){
    	/*CLIENT DEVIENT SERVEUR*/
    	int sock = socket(AF_INET, SOCK_STREAM, 0);
		if(sock == -1){
			perror("socket()");
			exit(-1);
		}
		sockaddr_in sin = { 0 };
		sin.sin_addr.s_addr = htonl(INADDR_ANY);
		sin.sin_family = AF_INET;
		sin.sin_port = htons(PORT_CLIENT);
		if(bind (sock, (sockaddr *) &sin, sizeof sin) == -1){
			perror("bind()");
			exit(-1);
		}
		if(listen(sock, 1) == -1){
			perror("listen()");
			exit(-1);
		}
		
		sockaddr_in csin = { 0 };
		//int csock;

		int sinsize = sizeof csin;
		printf("Recherche d'un adversaire en cours...\n");
		adversaire = accept(sock, (sockaddr *)&csin, &sinsize);

		if(adversaire == -1){
			perror("accept()");
			exit(-1);
		}else{
			printf("Adversaire trouvé.\n");
			partieEnCours = true;
			envoiGrille();
			receptionGrille();
			attente(adversaire);
		}
		
    }else{
    	/*Nouveau socket, client du joueur 1 (serveur)*/
		int sock = socket(AF_INET, SOCK_STREAM, 0);
		if(sock == -1){
			perror("socket()");
			exit(-1);
		}
		struct hostent *hostinfo = NULL;
		sockaddr_in sin = {0};

		hostinfo = gethostbyname(oponent);
		if (hostinfo == NULL){
			fprintf (stderr, "Unknown host %s.\n", oponent);
			exit(EXIT_FAILURE);
		}

		inet_aton(oponent, &sin.sin_addr);
		sin.sin_port = htons(PORT_CLIENT);
		sin.sin_family = AF_INET;

		if(connect(sock,(sockaddr *) &sin, sizeof(sockaddr)) == -1){
			perror("connect()");
			exit(-1);
		}else{
			printf("Connexion au client %s, %d\n", inet_ntoa(sin.sin_addr),ntohs(sin.sin_port));
			partieEnCours = true;
			adversaire = sock;
			receptionGrille();
			envoiGrille();
			lanceAttack(sock);
		}
   	}   
   	 
    exit(0);
}
