/*-----------------------------------------------------------
Client a lancer apres le serveur avec la commande :
client <adresse-serveur> <message-a-transmettre>
------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <linux/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netdb.h>
#include <strings.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

#include <errno.h>

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
int opGrid[GRID_WIDTH][GRID_HEIGHT];

void reinitOponentGrid(){
	for (int i=0; i<GRID_WIDTH;i++){
		for (int j=0; j<GRID_HEIGHT;j++){
			opGrid[i][j]=0;
		}
	}
}

void* prise_en_charge(int soc);

void lanceAttack(int soc){
	char pos[3];
	char buf[4] = "1";

	printf ("Choisissez des coordonnées d'attaque: ");
	scanf ("%3s",pos);  
	
	strcat(buf, pos);
	
	if ((write(soc, buf, 4)) < 0) {
		perror("erreur : impossible d'ecrire le message destine à l'autre joueur.");
		exit(1);
	}
	
	prise_en_charge(soc);
	
}

void updateOpGrid(PositionLetterDigit p, resultAttack res){
	Position pos= toPosition(p);

	if (res == WATER)
		opGrid[pos.x][pos.y]=-1;
	else if (res==TOUCH || res == SUNK || res == WIN)
		opGrid[pos.x][pos.y]=-2;

}

void* prise_en_charge(int soc)
{
    char buffer[10];
    
   	int l = read(soc, buffer, sizeof(buffer));
    if ( l < 0){
        return NULL;
    }

    if (buffer[0]== '0'){
    	printf("resultAttack recu\n");

    	resultAttack resA;
    	memcpy(&resA, buffer+sizeof(char), sizeof(resultAttack));
    	
    	PositionLetterDigit p;

	    memcpy( &p.letter, buffer+sizeof(resultAttack), sizeof(char) );

	    char subbuff[3];
	    memcpy( subbuff, buffer+sizeof(resultAttack)+sizeof(char), 2 );
	    subbuff[2] = '\0';
	    p.y = atoi(subbuff);

    	printf("attack en %c%d resultat %s\n", p.letter, p.y, toString(resA));

    	if (resA != REPEAT && resA!= ERROR){
    		updateOpGrid(p, resA);

    		printf("Grille de l'adversaire :\n");
    		printOponentGrid(opGrid);

    	}

    	prise_en_charge(soc);
    }
    else if (buffer[0] =='1'){
		printf("attaque recue\n");
		
		PositionLetterDigit p;
	    p.letter = buffer[1];
	    char subbuff[3];
	    memcpy( subbuff, &buffer[2], 2 );
	    subbuff[2] = '\0';
	    p.y = atoi(subbuff);
	  
	    resultAttack resA= attack(&grid, p);

    	printf("attaqué en %c%d resultat %s\n", p.letter, p.y, toString(resA));

    	printf("Votre grille :\n");
    	printGrid(grid);


    	if (resA != REPEAT && resA!= ERROR){

    		char res[10];
	    	res[0] = '0';

		    memcpy(res+sizeof(char), &resA, sizeof(resultAttack));
		    memcpy(res+sizeof(resultAttack), buffer+sizeof(char), 3*sizeof(char));
			
		    write(soc, res, 10);
		    
		    lanceAttack(soc); 
		}
		else{
			prise_en_charge(soc);
		}

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
    
    srand(time(NULL));
    init(&grid);
    reinitOponentGrid();
    
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

    int soc;
    
    if (strcmp(oponent, "game")==0){
    	printf("1er client attente\n");

    	sockaddr_in ad;
    	
		char machine[TAILLE_MAX_NOM+1]; 		/* nom de la machine locale */
		hostent* ptr_hote; 	
		
		gethostname(machine,TAILLE_MAX_NOM);	/* recuperation du nom de la machine */

		/* recuperation de la structure d'adresse en utilisant le nom */
		if ((ptr_hote = gethostbyname(machine)) == NULL) {
			perror("erreur : impossible de trouver le serveur a partir de son nom.");
			exit(1);
		}
		
		bcopy((char*)ptr_hote->h_addr, (char*)&ad, ptr_hote->h_length);
		ad.sin_family		= ptr_hote->h_addrtype; 			/* ou AF_INET */
		ad.sin_addr.s_addr	= INADDR_ANY; 						/* ou AF_INET */

		ad.sin_port = htons(PORT_CLIENT);

    	/* creation de la socket */
	    if ((soc = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
	        perror("erreur : impossible de creer la socket de connexion avec le client.");
	        exit(1);
	    }

	    int reuse = 1;
		setsockopt(soc, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

	    /* association du socket soc à la structure d'adresse adresse_locale */
	    if ((bind(soc, (sockaddr*)(&(ad)), sizeof(ad))) < 0) {
	        perror("erreur : impossible de lier la socket a l'adresse de connexion.");
	        exit(1);
	    }
	    
	    /* initialisation de la file d'ecoute */
	    listen(soc,1);

		sockaddr_in adresse_client_courant;
	    
	    int longueur_adresse_courante = sizeof(adresse_client_courant);
	    int nouv_soc;

		printf("Attente de joueur...\n");
        /* adresse_client_courant sera renseigné par accept via les infos du connect */
        //for (;;){
	        if ((nouv_soc = 
	            accept(soc, 
	                   (sockaddr*)(&adresse_client_courant),
	                   &longueur_adresse_courante))
	             < 0) {
	            perror("erreur : impossible d'accepter la connexion avec le client.");
	            exit(1);
	        }

	        printf("Votre grille :\n");
    		printGrid(grid);
    		printf("Grille de l'adversaire :\n");
    		printOponentGrid(opGrid);

        	prise_en_charge(nouv_soc);
        //}

    }else{
    	printf("2eme client joue contre %s\n", oponent);
    	sockaddr_in ad;
    	hostent * ptr_host; 		/* info sur une machine hote */
    	
    	if ((ptr_host = gethostbyname(oponent)) == NULL) {
			perror("erreur : impossible de trouver le serveur a partir de son adresse.");
			exit(1);
		}
		
		/* copie caractere par caractere des infos de ptr_host vers adresse_locale_globale */
		bcopy((char*)ptr_host->h_addr, (char*)&ad.sin_addr, ptr_host->h_length);
		ad.sin_family = AF_INET; /* ou ptr_host->h_addrtype; */
		
		ad.sin_port = htons(PORT_CLIENT);

		printf("numero de port pour la connexion à l'autre joueur : %d \n", ntohs(ad.sin_port));
		
		if ((soc = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
			perror("erreur : impossible de creer la socket de connexion avec le serveur.");
			exit(1);
		}
		
		if ((connect(soc, (sockaddr*)(&ad), sizeof(ad))) < 0) {
			perror("erreur : impossible de se connecter au serveur.");
			exit(1);
		}

		printf("Votre grille :\n");
    	printGrid(grid);
		printf("Grille de l'adversaire :\n");
		printOponentGrid(opGrid);
		
		lanceAttack(soc);
		
   	}
    close(soc);
    close(socket_descriptor);
    
    exit(0);
    
}
