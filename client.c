/*-----------------------------------------------------------
Client a lancer apres le serveur avec la commande :
client <adresse-serveur> <message-a-transmettre>
------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <linux/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <strings.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>

typedef struct sockaddr 	sockaddr;
typedef struct sockaddr_in 	sockaddr_in;
typedef struct hostent 		hostent;
typedef struct servent 		servent;

void get_grille_courante(int socket_descriptor, char* buffer, long taille){
	int longueur = read(socket_descriptor, buffer, taille);
	if(longueur > 0){
		printf("reponse du serveur : \n");
		write(1,buffer,longueur);
	}
}


void* interaction_client(void* ad)
{
	//cast adresse_locale
	sockaddr_in *tmp = (sockaddr_in*) (ad);
	sockaddr_in adresse_locale = (sockaddr_in) (*tmp) ;
	
	bool play = true;
	int socket_descriptor;
	char buffer[2560];
	
	while (play){
		/* creation de la socket */
		if ((socket_descriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
			perror("erreur : impossible de creer la socket de connexion avec le serveur.");
			exit(1);
		}
		
		/* tentative de connexion au serveur dont les infos sont dans adresse_locale */
		if ((connect(socket_descriptor, (sockaddr*)(&adresse_locale), sizeof(adresse_locale))) < 0) {
			perror("erreur : impossible de se connecter au serveur.");
			exit(1);
		}
		
		/* envoi du message vers le serveur */
		char pos[3];
		char buf[4] = "1";

		printf ("Enter attack position: ");
		scanf ("%3s",pos);  
		
		strcat(buf, pos);
		
		if ((write(socket_descriptor, buf, 4)) < 0) {
			perror("erreur : impossible d'ecrire le message destine au serveur.");
			exit(1);
		}
		
		get_grille_courante(socket_descriptor, buffer, sizeof(buffer));
	}
}

int main(int argc, char **argv) {
  
    int socket_descriptor; 		/* descripteur de socket */
	int longueur; 				/* longueur d'un buffer utilisé */
    sockaddr_in adresse_locale; /* adresse de socket local */
    hostent * ptr_host; 		/* info sur une machine hote */
    servent * ptr_service; 		/* info sur service */
    char buffer[2560];
    char * prog; 				/* nom du programme */
    char * host; 				/* nom de la machine distante */
     
    if (argc != 2) {
		perror("usage : client <adresse-serveur>");
		exit(1);
    }
   
    prog = argv[0];
    host = argv[1];
    
    printf("nom de l'executable : %s \n", prog);
    printf("adresse du serveur  : %s \n", host);
    
    if ((ptr_host = gethostbyname(host)) == NULL) {
		perror("erreur : impossible de trouver le serveur a partir de son adresse.");
		exit(1);
    }
    
    /* copie caractere par caractere des infos de ptr_host vers adresse_locale */
    bcopy((char*)ptr_host->h_addr, (char*)&adresse_locale.sin_addr, ptr_host->h_length);
    adresse_locale.sin_family = AF_INET; /* ou ptr_host->h_addrtype; */
    
    /* 2 facons de definir le service que l'on va utiliser a distance */
    /* (commenter l'une ou l'autre des solutions) */
    
    /*-----------------------------------------------------------*/
    /* SOLUTION 1 : utiliser un service existant, par ex. "irc" */
    /*
    if ((ptr_service = getservbyname("irc","tcp")) == NULL) {
	perror("erreur : impossible de recuperer le numero de port du service desire.");
	exit(1);
    }
    adresse_locale.sin_port = htons(ptr_service->s_port);
    */
    /*-----------------------------------------------------------*/
    
    /*-----------------------------------------------------------*/
    /* SOLUTION 2 : utiliser un nouveau numero de port */
    adresse_locale.sin_port = htons(5000);
    /*-----------------------------------------------------------*/
    
    printf("numero de port pour la connexion au serveur : %d \n", ntohs(adresse_locale.sin_port));
    
    /* creation de la socket */
    if ((socket_descriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("erreur : impossible de creer la socket de connexion avec le serveur.");
		exit(1);
    }
    
    /* tentative de connexion au serveur dont les infos sont dans adresse_locale */
    if ((connect(socket_descriptor, (sockaddr*)(&adresse_locale), sizeof(adresse_locale))) < 0) {
		perror("erreur : impossible de se connecter au serveur.");
		exit(1);
    }
    
    printf("connexion etablie avec le serveur. \n");
    
    printf("envoi d'un message get au serveur. \n");
      
    /* envoi du message vers le serveur */
    if ((write(socket_descriptor, "0", 2)) < 0) {
		perror("erreur : impossible d'ecrire le message destine au serveur.");
		exit(1);
    }
    
    printf("message get envoye au serveur. \n");
    
    get_grille_courante(socket_descriptor, buffer, sizeof(buffer));
	
    printf("Lancement thread attack. \n");
    
   	pthread_t nouv_client;
    if (pthread_create(&nouv_client, NULL, interaction_client, (sockaddr_in*) &adresse_locale))
    {
        perror("Impossible creer thread");
        return -1;
    }
	
    if (pthread_join(nouv_client, NULL)){
        perror("Impossible joindre thread");
        return -1;
    }
    
    close(socket_descriptor);
    
    printf("connexion avec le serveur fermee, fin du programme.\n");
    
    exit(0);
    
}
