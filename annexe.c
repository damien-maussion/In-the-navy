#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <linux/types.h> 	/* pour les sockets */
#include <sys/socket.h>
#include <netdb.h>			/* pour hostent, servent */
#include <strings.h>
#include <pthread.h>

typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;

struct args_lance_listener{
	sockaddr_in ad;
	void *(*traitement)(void *);
};
typedef struct args_lance_listener args_lance_listener;

struct args_traitement{
	int soc;
	sockaddr_in ad;
};
typedef struct args_traitement args_traitement;

void* lance_listener(void* void_args ){

	//cast 
    args_lance_listener *args = void_args;

    int socket_descriptor;              /* descripteur de socket */
    int nouv_socket_descriptor;         /* [nouveau] descripteur de socket */
    int longueur_adresse_courante;      /* longueur d'adresse courante d'un client */
    sockaddr_in adresse_client_courant; /* adresse client courant */

    /* creation de la socket */
    if ((socket_descriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("erreur : impossible de creer la socket de connexion avec le client.");
        exit(1);
    }

    /* association du socket socket_descriptor à la structure d'adresse adresse_locale */
    if ((bind(socket_descriptor, (sockaddr*)(&(args->ad)), sizeof(args->ad))) < 0) {
        perror("erreur : impossible de lier la socket a l'adresse de connexion.");
        exit(1);
    }
    
    /* initialisation de la file d'ecoute */
    listen(socket_descriptor,5);

    /* attente des connexions et traitement des donnees recues */
    for(;;) {
    
        longueur_adresse_courante = sizeof(adresse_client_courant);
        
        /* adresse_client_courant sera renseigné par accept via les infos du connect */
        if ((nouv_socket_descriptor = 
            accept(socket_descriptor, 
                   (sockaddr*)(&adresse_client_courant),
                   &longueur_adresse_courante))
             < 0) {
            perror("erreur : impossible d'accepter la connexion avec le client.");
            exit(1);
        }
        
        /* traitement du message */
        printf("reception d'un message.\n");
        
        args_traitement args_t;
        args_t.soc = nouv_socket_descriptor;
        args_t.ad = adresse_client_courant;

        //args->traitement((args_traitement*)&args_t);
        
        pthread_t nouv_client;
    
        if (pthread_create(&nouv_client, NULL, args->traitement, (args_traitement*)&args_t))
        {
            perror("Impossible creer thread");
            exit(1);
        }
    }
}
