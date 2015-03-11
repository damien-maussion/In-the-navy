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
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

#include "../grid.h"
#include "../annexe.c"

#define TAILLE_MAX_NOM 256
#define PORT_CLIENT 5001

typedef struct sockaddr 	sockaddr;
typedef struct sockaddr_in 	sockaddr_in;
typedef struct hostent 		hostent;
typedef struct servent 		servent;

sockaddr_in adresse_locale_globale; /* adresse de socket local */

Grid grid;

void lanceAttack(int soc){
	char pos[3];
	char buf[4] = "1";

	printf ("Choisissez des coordonnées d'attaque: ");
	scanf ("%3s",pos);  
	
	strcat(buf, pos);
	
	if ((write(soc, buf, 4)) < 0) {
		perror("erreur : impossible d'ecrire le message destine au serveur.");
		exit(1);
	}
	
	
	
}

void* prise_en_charge(void *args)
{
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
    }
    else if (c=='1'){ 			//attack 
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
}

void byebye(void){

	/*sockaddr_in *tmp = (sockaddr_in*) (ad);
	sockaddr_in adresse_locale = (sockaddr_in) (*tmp) ;*/
	
	int socket_descriptor;
	char buffer[2560];
	/* creation de la socket */
	if ((socket_descriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("erreur : impossible de creer la socket de connexion avec le serveur.");
		exit(1);
	}
	
	/* tentative de connexion au serveur dont les infos sont dans adresse_locale */
	if ((connect(socket_descriptor, (sockaddr*)(&adresse_locale_globale), sizeof(adresse_locale_globale))) < 0) {
		perror("erreur : impossible de se connecter au serveur.");
		exit(1);
	}
	
	/* envoi du message vers le serveur */
	char buf[4] = "2";
	
	if ((write(socket_descriptor, buf, 4)) < 0) {
		perror("erreur suppr client : impossible d'ecrire le message destine au serveur.");
		exit(1);
	}else{
		printf("\nVous êtes désormais déconnecté(e) du serveur.\n");
	}
}

void ctrlC_Handler(int e) {
    exit(0);
}

int main(int argc, char **argv) {
  
    int socket_descriptor; 		/* descripteur de socket */
	int longueur; 				/* longueur d'un buffer utilisé */
    hostent * ptr_host; 		/* info sur une machine hote */
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
    
    if ((ptr_host = gethostbyname(host)) == NULL) {
		perror("erreur : impossible de trouver le serveur a partir de son adresse.");
		exit(1);
    }
    
    /* copie caractere par caractere des infos de ptr_host vers adresse_locale_globale */
    bcopy((char*)ptr_host->h_addr, (char*)&adresse_locale_globale.sin_addr, ptr_host->h_length);
    adresse_locale_globale.sin_family = AF_INET; /* ou ptr_host->h_addrtype; */
    
    /* 2 facons de definir le service que l'on va utiliser a distance */
    /* (commenter l'une ou l'autre des solutions) */
    
    /*-----------------------------------------------------------*/
    /* SOLUTION 1 : utiliser un service existant, par ex. "irc" */
    /*
    if ((ptr_service = getservbyname("irc","tcp")) == NULL) {
	perror("erreur : impossible de recuperer le numero de port du service desire.");
	exit(1);
    }
    adresse_locale_globale.sin_port = htons(ptr_service->s_port);
    */
    /*-----------------------------------------------------------*/
    
    /*-----------------------------------------------------------*/
    /* SOLUTION 2 : utiliser un nouveau numero de port */
    adresse_locale_globale.sin_port = htons(5000);
    /*-----------------------------------------------------------*/

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
    
    init(&grid);
    
    printf("envoi d'un message getoponent au serveur. \n");
      
    /* envoi du message vers le serveur */
    if ((write(socket_descriptor, "", 2)) < 0) {
		perror("erreur : impossible d'ecrire le message destine au serveur.");
        exit(1);
    }
    
    char oponent[50] ="";
    if ((read(socket_descriptor, oponent, 50)) < 0) {
		perror("erreur : impossible de lire le message du serveur.");
        exit(1);
    }
    
    args_lance_listener args;
    	
	char machine[TAILLE_MAX_NOM+1]; 	/* nom de la machine locale */
	hostent* ptr_hote; 	
	
	gethostname(machine,TAILLE_MAX_NOM);	/* recuperation du nom de la machine */

	/* recuperation de la structure d'adresse en utilisant le nom */
	if ((ptr_hote = gethostbyname(machine)) == NULL) {
		perror("erreur : impossible de trouver le serveur a partir de son nom.");
		exit(1);
	}
	
	bcopy((char*)ptr_hote->h_addr, (char*)&args.ad.sin_addr, ptr_hote->h_length);
	args.ad.sin_family		= ptr_hote->h_addrtype; 			/* ou AF_INET */
	args.ad.sin_addr.s_addr	= INADDR_ANY; 						/* ou AF_INET */

	args.ad.sin_port = htons(PORT_CLIENT);
	printf("numero de port : %d \n", 
	   ntohs(args.ad.sin_port) /*ntohs(ptr_service->s_port)*/);
	  printf("%s\n", inet_ntoa(args.ad.sin_addr));
	  
	args.traitement = prise_en_charge;
	printf("Lancement de l'écoute\n");
	
	lance_listener((args_lance_listener*) &args);
    
    if (strcmp(oponent, "game")==0){
    	printf("1er client attente");
    	
		
    }else{
    	printf("2eme client joue contre %s\n", oponent);
    	int socket_descriptor2;
    	
    	if ((ptr_host = gethostbyname(oponent)) == NULL) {
			perror("erreur : impossible de trouver le serveur a partir de son adresse.");
			exit(1);
		}
		
		/* copie caractere par caractere des infos de ptr_host vers adresse_locale_globale */
		bcopy((char*)ptr_host->h_addr, (char*)&adresse_locale_globale.sin_addr, ptr_host->h_length);
		adresse_locale_globale.sin_family = AF_INET; /* ou ptr_host->h_addrtype; */
		
		adresse_locale_globale.sin_port = htons(PORT_CLIENT);

		printf("numero de port pour la connexion au serveur : %d \n", ntohs(adresse_locale_globale.sin_port));
		
		/* creation de la socket */
		if ((socket_descriptor2 = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
			perror("erreur : impossible de creer la socket de connexion avec le serveur.");
			exit(1);
		}
		
		/* tentative de connexion au serveur dont les infos sont dans adresse_locale_globale */
		if ((connect(socket_descriptor2, (sockaddr*)(&adresse_locale_globale), sizeof(adresse_locale_globale))) < 0) {
			perror("erreur : impossible de se connecter au serveur.");
			exit(1);
		}
    	
    	/*
    	sockaddr_in ad;
    	inet_aton(oponent, &ad.sin_addr);
    	ad.sin_port = htons(PORT_CLIENT);
    	printf("%s\n", inet_ntoa(ad.sin_addr));

	
		if ((socket_descriptor2 = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
			perror("erreur : impossible de creer la socket de connexion avec le serveur.");
			exit(1);
		}


		if ((connect(socket_descriptor2, (sockaddr*)(&(ad)), sizeof(ad))) < 0) {
			perror("erreur : impossible de se connecter au serveur.");
			exit(1);
		}
		*/
		
		lanceAttack(socket_descriptor2);
		
   	}
    
    
    exit(0);
    
}
