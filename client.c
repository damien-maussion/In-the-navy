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

#include "grid.h"
#include "annexe.c"

#define TAILLE_MAX_NOM 256
#define PORT_SERVER 5001

typedef struct sockaddr 	sockaddr;
typedef struct sockaddr_in 	sockaddr_in;
typedef struct hostent 		hostent;
typedef struct servent 		servent;

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
		
		//get_grille_courante(socket_descriptor, buffer, sizeof(buffer));
        /*
        int longueur = read(socket_descriptor, buffer, sizeof(buffer));
        if(longueur > 0){
            printf("reponse du serveur : \n");
            Trame t = deserializeTrame(buffer);
            printf("%s", t.data);
        }
        */
	}
}


TrameBuffer tb;
pthread_mutex_t mutex_trame_buffer;

void* listen_server(void* args)
{
    //cast
    args_traitement *args_t = args;
    printf("lis beg \n");

    char buffer[TAILLE_MAX_TRAME];   
    if (read(args_t->soc, buffer, sizeof(buffer)) > 0){
        //printf("reponse du serveur : \n");
        Trame t = deserializeTrame(buffer);

        pthread_mutex_lock(&mutex_trame_buffer);
        receveTrame(&tb, t);
        pthread_mutex_unlock(&mutex_trame_buffer);
        
        //printf("Trame data : \n%s\nidTtrame : %d\tindex: %d\ttaille : %d\n", t.data, t.idTrame, t.index, t.taille);

        //printf("TB data : \n%s\nidTtrame : %d\tnbTrameReceved: %d\tfinish : %d\n", tb.data, tb.idTrame, tb.nbTrameReceved, tb.finish);

        printf("%d\n",tb.finish );
        if (tb.finish){
            if (tb.data[0]==0){
                ResponseGet res = deserializeResponseGet(tb.data);
                printf("Grille Get:\n");
                printOponentGrid(res.grid);
            }
            else{
                ResponseAttack res = deserializeResponseAttack(tb.data);
                printf("Grille attack:\n");
                printOponentGrid(res.grid);
            }
        }
        //write(1,t.data,longueur);
    }
    printf(" lis end\n");
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

    printf("Lancement thread d'écoute sur le port 5001. \n");

    tb.idTrame=-1;
    args_lance_listener args;

	sockaddr_in adresse_locale2;
	char machine[TAILLE_MAX_NOM+1]; 	/* nom de la machine locale */
	hostent* ptr_hote; 	
    
    gethostname(machine,TAILLE_MAX_NOM);	/* recuperation du nom de la machine */

	/* recuperation de la structure d'adresse en utilisant le nom */
    if ((ptr_hote = gethostbyname(machine)) == NULL) {
		perror("erreur : impossible de trouver le serveur a partir de son nom.");
		exit(1);
    }
	
	/* initialisation de la structure adresse_locale avec les infos recuperees */			
    
    /* copie de ptr_hote vers adresse_locale */
    bcopy((char*)ptr_hote->h_addr, (char*)&adresse_locale2.sin_addr, ptr_hote->h_length);
    adresse_locale2.sin_family		= ptr_hote->h_addrtype; 			/* ou AF_INET */
    adresse_locale2.sin_addr.s_addr	= INADDR_ANY; 						/* ou AF_INET */

    adresse_locale2.sin_port = htons(PORT_SERVER);

    args.ad = adresse_locale2;
    
    args.traitement = listen_server;

    if (pthread_mutex_init(&mutex_trame_buffer, NULL) != 0)
    {
        perror("\n mutex_trame_buffer init failed\n");
        return 1;
    }

    pthread_t thread_listen;
    if (pthread_create(&thread_listen, NULL, lance_listener, (args_lance_listener*) &args))
    {
        perror("Impossible creer thread listen");
        return -1;
    }
    sleep(1);
    
    printf("envoi d'un message get au serveur. \n");
      
    /* envoi du message vers le serveur */
    if ((write(socket_descriptor, "0", 2)) < 0) {
		perror("erreur : impossible d'ecrire le message destine au serveur.");
        exit(1);
    }
    
    //close(socket_descriptor);
    printf("message get envoye au serveur. \n");
    
    //get_grille_courante(socket_descriptor, buffer, sizeof(buffer));
    
    printf("Lancement thread attack. \n");
    args.ad.sin_port = htons(5000);
    
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
    pthread_mutex_destroy(&mutex_trame_buffer);
    
    exit(0);
    
}
