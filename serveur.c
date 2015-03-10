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

#include "grid.h" 
#include "annexe.c" 

#define TAILLE_MAX_NOM 256
#define PORT_SERVER 5000

typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;
typedef struct hostent hostent;
typedef struct servent servent;

typedef struct s_List Liste_clients;
struct s_List
{
    Liste_clients *next;
    sockaddr_in ad;
};

Grid g;
Liste_clients *clients = NULL;
pthread_mutex_t mutex_grid;


void sendTrame(Trame t){
    Liste_clients *ad_cour = clients;
    char *str_trame = serializeTrame(t);
    int length_trame = TAILLE_MAX_DATA_TRAME + 3*sizeof(int);

    //printf("Trame data : \n%s\nidTtrame : %d\tindex: %d\ttaille : %d\n", t.data, t.idTrame, t.index, t.taille);
    printf("send Trame %d-%d\n", t.idTrame, t.index);

    while (ad_cour !=NULL){

        int socket_descriptor;

        /* creation de la socket */
        if ((socket_descriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            perror("erreur : impossible de creer la socket de connexion avec le serveur.");
            exit(1);
        }
        
        /* tentative de connexion au serveur dont les infos sont dans adresse_locale */
        if ((connect(socket_descriptor, (sockaddr*)(&(ad_cour->ad)), sizeof(ad_cour->ad))) < 0) {
            perror("erreur : impossible de se connecter au serveur.");
            exit(1);
        }

        write(socket_descriptor, str_trame, length_trame);
        ad_cour = ad_cour->next;
    }
}

void sendTrameTo(Trame t, sockaddr_in add){
    char *str_trame = serializeTrame(t);
    int length_trame = TAILLE_MAX_DATA_TRAME + 3*sizeof(int);

    //printf("Trame data : \n%s\nidTtrame : %d\tindex: %d\ttaille : %d\n", t.data, t.idTrame, t.index, t.taille);
    printf("send Trame %d-%d\n", t.idTrame, t.index);

    int socket_descriptor;

    /* creation de la socket */
    if ((socket_descriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("erreur : impossible de creer la socket de connexion avec le serveur.");
        exit(1);
    }
    
    /* tentative de connexion au serveur dont les infos sont dans adresse_locale */
    if ((connect(socket_descriptor, (sockaddr*)(&(add)), sizeof(add))) < 0) {
        perror("erreur : impossible de se connecter au serveur.");
        exit(1);
    }

    write(socket_descriptor, str_trame, length_trame);
        
}

int idTrame = 0;
void sendResponse(char * str, int length){
    int offset = 0;

    //printf("resp : %d - %d\n%s\n", (int) strlen(str), getGridStringLength(), str);

    while (offset < length){
        Trame t;
        t.idTrame = idTrame;
        t.index = offset;
        t.taille = length;
        memcpy(t.data, str+offset, TAILLE_MAX_DATA_TRAME);
        offset+= TAILLE_MAX_DATA_TRAME;
        //write(sd,serializeTrame(t),TAILLE_MAX_DATA_TRAME + 3*sizeof(int));
        sendTrame(t);
    }
    idTrame++;
}

void sendResponseTo(char * str, int length, sockaddr_in add){
    int offset = 0;

    //printf("resp : %d - %d\n%s\n", (int) strlen(str), getGridStringLength(), str);

    while (offset < length){
        Trame t;
        t.idTrame = idTrame;
        t.index = offset;
        t.taille = length;
        memcpy(t.data, str+offset, TAILLE_MAX_DATA_TRAME);
        offset+= TAILLE_MAX_DATA_TRAME;
        //write(sd,serializeTrame(t),TAILLE_MAX_DATA_TRAME + 3*sizeof(int));
        sendTrameTo(t, add);
    }
    idTrame++;
}


/* POURQUOI CETTE FONCTION SORT UNE GRILLE AU LIEU DE LA STRING D'EXTINCTION DE SERVEUR ??? (VOIR S'IL Y A UN LIEN AVEC LE PROBLEME COURANT)*/
void BroadCast(char* str, int length){

	int offset = 0;
    while (offset < length){
        Trame t;
        t.idTrame = idTrame;
        t.index = offset;
        t.taille = length;
        memcpy(t.data, str+offset, TAILLE_MAX_DATA_TRAME);
        offset+= TAILLE_MAX_DATA_TRAME;
		Liste_clients *ad_cour = clients;
		int length_trame = TAILLE_MAX_DATA_TRAME + 3*sizeof(int);
		while (ad_cour !=NULL){

			int socket_descriptor;

			/* creation de la socket */
			if ((socket_descriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
				perror("erreur : impossible de creer la socket de connexion avec le serveur.");
				exit(1);
			}
	
			/* tentative de connexion au serveur dont les infos sont dans adresse_locale */
			if ((connect(socket_descriptor, (sockaddr*)(&(ad_cour->ad)), sizeof(ad_cour->ad))) < 0) {
				perror("erreur : impossible de se connecter au serveur.");
				exit(1);
			}

			write(socket_descriptor, t.data, length_trame);
			ad_cour = ad_cour->next;
		}
    }
    idTrame++;
}

int sockaddr_equal(struct sockaddr_in sin1, struct sockaddr_in sin2){
	return (sin1.sin_family == sin2.sin_family && sin1.sin_addr.s_addr == sin2.sin_addr.s_addr);
}

void* prise_en_charge_client(void* args)
{
    //cast du socket
    args_traitement *args_t = args;

    char buffer[256];
    int longueur;
   
    if ((longueur = read(args_t->soc, buffer, sizeof(buffer))) <= 0) 
        return NULL;
    
    char c= buffer[0];
    if (c=='0'){

        args_t->ad.sin_port = htons(5001);

        //Ajout en tête
        Liste_clients *nouv_client = malloc(sizeof(Liste_clients));
        if (nouv_client){
           nouv_client->ad = args_t->ad;
           nouv_client->next = clients;
        }
        clients = nouv_client;

        printf("get %s\n", inet_ntoa(args_t->ad.sin_addr));

        //write(args_t->soc,getOponentGrid(g),getGridStringLength());

        ResponseGet res;
        //pthread_mutex_lock(&mutex_grid);
        getOponentGrid(g, res.grid);
        //pthread_mutex_unlock(&mutex_grid);
        //char *str_grid = getOponentGrid(g);
        //sendResponse(serializeResponseGet(res),TAILLE_RESPONSE);
        sendResponseTo(serializeResponseGet(res),TAILLE_RESPONSE,args_t->ad);

        //delete str_grid;
    }else if (c=='1'){
        PositionLetterDigit p;
        p.letter = buffer[1];
        char subbuff[3];
        memcpy( subbuff, &buffer[2], 2 );
        subbuff[2] = '\0';
        p.y = atoi(subbuff);
        
        /*
        //write(args_t->soc,getOponentGrid(g),getGridStringLength());
        //sendResponse(getOponentGrid(g),getGridStringLength());
        char *str_grid = getOponentGrid(g);
        sendResponse(str_grid,(int) strlen(str_grid));
        //delete str_grid;
        */

        ResponseAttack res;
        pthread_mutex_lock(&mutex_grid);
        res.result = attack(&g, p);
        res.who = args_t->ad.sin_addr;
        getOponentGrid(g, res.grid);
        printGrid(g);
        pthread_mutex_unlock(&mutex_grid);
                
        printf("attack\n");

        sendResponse(serializeResponseAttack(res), TAILLE_RESPONSE);
    }else if(c=='2'){     
    
        /*Liste_clients *tmp1 = clients;
        while(tmp1){
        	printf("DELETE %s\n", inet_ntoa(tmp1->ad.sin_addr));
        	tmp1 = tmp1->next;
        }*/
           
        Liste_clients *nouv_client = clients;
        
        if(sockaddr_equal(nouv_client->ad, args_t->ad)){
        	clients = clients->next;
        	free(nouv_client);
        }else{
			Liste_clients *it = clients->next;
        
		    while(it->next && sockaddr_equal(it->ad, args_t->ad)){
		    	nouv_client = nouv_client->next;
		    	it = it->next;
		    }
		    
		    if(sockaddr_equal(it->ad, args_t->ad)){
		    	nouv_client->next = it->next;
		    	free(it);
		    }else{
		    	perror("erreur : ce client n'existe pas.");
		    }
		}
        
        /*Liste_clients *tmp = clients;
        while(tmp){
        	printf("DELETE %s\n", inet_ntoa(tmp->ad.sin_addr));
        	tmp = tmp->next;
        }*/
    }

    return NULL;
    
}

void byebye(void){
	
	char *str = "\nLe serveur est désormais hors-ligne.\n";
	BroadCast(str, strlen(str));
	Liste_clients *it = clients;
    while(it->next){
    	Liste_clients *tmp = it;
    	it = it->next;
    	free(tmp);
    }
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
    
    printf("numero de port pour la connexion au serveur : %d \n", 
		   ntohs(adresse_locale.sin_port) /*ntohs(ptr_service->s_port)*/);


    //creation grille

    // seed ...
    init(&g);
    printGrid(g);

	if (pthread_mutex_init(&mutex_grid, NULL) != 0)
    {
        perror("\n mutex_grid init failed\n");
        return 1;
    }
        
    printf("Lancement de l'écoute\n");

    args_lance_listener args;
    args.ad = adresse_locale;
    args.traitement = prise_en_charge_client;

    lance_listener((args_lance_listener*) &args);
    pthread_mutex_destroy(&mutex_grid);

    /* attente des connexions et traitement des donnees recues */
    /*
    for(;;) {
    
		longueur_adresse_courante = sizeof(adresse_client_courant);
		
		// adresse_client_courant sera renseigné par accept via les infos du connect 
		if ((nouv_socket_descriptor = 
			accept(socket_descriptor, 
			       (sockaddr*)(&adresse_client_courant),
			       &longueur_adresse_courante))
			 < 0) {
			perror("erreur : impossible d'accepter la connexion avec le client.");
			exit(1);
		}
		
		// traitement du message
		printf("reception d'un message.\n");

        pthread_t nouv_client;
        if (pthread_create(&nouv_client, NULL, prise_en_charge_client, (int*)&nouv_socket_descriptor))
        {
            perror("Impossible creer thread");
            return -1;
        }

    }
    */   
}


