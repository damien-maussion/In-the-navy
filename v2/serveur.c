/*----------------------------------------------
Serveur à lancer avant le client
------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <linux/types.h> 	/* pour les sockets */
#include <sys/socket.h>
#include <netdb.h>			/* pour hostent, servent */
#include <strings.h> 		/* pour bcopy, ... */ 
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

#include "../grid.h" 
#include "../annexe.c" 

#define TAILLE_MAX_NOM 256
#define PORT_SERVER 5000

typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;
typedef struct hostent hostent;
typedef struct servent servent;

in_addr client;
bool isInGame = false;

int sockaddr_equal(struct sockaddr_in sin1, struct sockaddr_in sin2){
	return (sin1.sin_family == sin2.sin_family && sin1.sin_addr.s_addr == sin2.sin_addr.s_addr);
}

void* prise_en_charge_client(void* args)
{
    //cast du socket
    args_traitement *args_t = args;

    int longueur;
    char buff[50];
   
    if ((longueur = read(args_t->soc, buff, sizeof(buff))) <= 0) 
        return NULL;
        
    if (!isInGame){
    	 isInGame=true;
    	 client = args_t->ad.sin_addr;
    	 write(args_t->soc, "game", 4);
    }
    else{
    	write(args_t->soc, inet_ntoa(client), strlen(inet_ntoa(client)));
    	isInGame=false;
    }
    
    
    return NULL;
    
}

void byebye(void){
	
	char *str = "-Le serveur est désormais hors-ligne.\n\0";
}

void ctrlC_Handler(int e){
    exit(0);
}


/*------------------------------------------------------*/



/*------------------------------------------------------*/
int main(int argc, char **argv) {
  
    int socket_descriptor;				/* descripteur de socket */
	int nouv_socket_descriptor;			/* [nouveau] descripteur de socket */
	int longueur_adresse_courante;		/* longueur d'adresse courante d'un client */
    sockaddr_in adresse_locale;			/* structure d'adresse locale*/
	sockaddr_in	adresse_client_courant;	/* adresse client courant */
    hostent* ptr_hote; 					/* les infos recuperees sur la machine hote */
    servent* ptr_service; 				/* les infos recuperees sur le service de la machine */
    char machine[TAILLE_MAX_NOM+1]; 	/* nom de la machine locale */
    
    gethostname(machine,TAILLE_MAX_NOM);	/* recuperation du nom de la machine */
    
    atexit(byebye);
    signal(SIGINT, ctrlC_Handler);
    
    /* recuperation de la structure d'adresse en utilisant le nom */
    if ((ptr_hote = gethostbyname(machine)) == NULL) {
		perror("erreur : impossible de trouver le serveur a partir de son nom.");
		exit(1);
    }
    
    /* initialisation de la structure adresse_locale avec les infos recuperees */			
    
    /* copie de ptr_hote vers adresse_locale */
    bcopy((char*)ptr_hote->h_addr, (char*)&adresse_locale.sin_addr, ptr_hote->h_length);
    adresse_locale.sin_family		= ptr_hote->h_addrtype; 	/* ou AF_INET */
    adresse_locale.sin_addr.s_addr	= INADDR_ANY; 			/* ou AF_INET */

    adresse_locale.sin_port = htons(PORT_SERVER);
    /*-----------------------------------------------------------*/
    
    printf("numero de port pour la connexion au serveur : %d \n", ntohs(adresse_locale.sin_port) /*ntohs(ptr_service->s_port)*/);
        
    printf("Lancement de l'écoute\n");

    args_lance_listener args;
    args.ad = adresse_locale;
    args.traitement = prise_en_charge_client;

    lance_listener((args_lance_listener*) &args);

}


