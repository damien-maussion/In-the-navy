/*-----------------------------------------------------------
Client a lancer apres le serveur avec la commande :
./client.exe <adresse-serveur>
------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#include "client.h"

/*void reinitOponentGrid(){
	//réinitialisation de la grille
	for (int i=0; i<GRID_WIDTH;i++){
		for (int j=0; j<GRID_HEIGHT;j++){
			opGrid[i][j]=0;
		}
	}
}*/

/*void updateOpGrid(PositionLetterDigit p, resultAttack res){
	//mise-à-jour de la grille adverse après attaque
	Position pos= toPosition(p);
	if (res == WATER)
		opGrid[pos.x][pos.y]=-1;
	else if (res==TOUCH || res == SUNK || res == WIN)
		opGrid[pos.x][pos.y]=-2;
}*/

void lanceAttaque(int sock){
	char pos[3];
	char buffer[4] = "1";

	scanf ("%3s",pos);  
	
	strcat(buffer, pos);

	//envoi des coordonnées d'attaque à l'adversaire
	if(send(sock, buffer, strlen(buffer), 0) < 0){
		perror("send()");
		exit(-1);
	}
}

void connexionAuServeur(){

	if(sock_client != -1){
		close(sock_client);
		sock_client = -1;
	}
		
	//Creation de la socket
    if ((serveur = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("erreur : impossible de creer la socket de connexion avec le serveur.");
		exit(1);
    }

	//Tentative de connexion au serveur dont les infos sont dans adresse_locale_globale
    if ((connect(serveur, (sockaddr*)(&adresse_locale_globale), sizeof(adresse_locale_globale))) < 0) {
		perror("erreur : impossible de se connecter au serveur.");
		exit(1);
    }
    
    printf("Connexion au serveur établie. \n");
    
	tb.idTrame=-1;
    tb.finish=false;

	pthread_t thread_listen;
    if (pthread_create(&thread_listen, NULL, ecoute, NULL)){
        perror("Impossible creer thread listen");
        exit(-1);
    }
    
    while(1){
    	lanceAttaque(serveur);
    }
	
	if (pthread_join(thread_listen, NULL)){
        perror("Impossible joindre thread");
        exit(-1);
    }
}

void* ecoute(){

    //cast
    //args_traitement *args_t = args;
    char buffer[TAILLE_MAX_TRAME];
    
    while(1){

		if (recv(serveur, buffer, sizeof(buffer), 0) >= 0){
			Trame t = deserializeTrame(buffer);
			receveTrame(&tb, t);
			puts("cc");
			if (tb.finish){
			
				if(tb.data[0] == '-'){				//serveur hors-ligne
					printf("%s\n",buffer);
					exit(0);
				/*}else if(buffer[0] == '1'){			//attaque
					printf("Attaque en %s\n",buffer);
				*/
				}else if (tb.data[0]== '0'){
					//printf("%s\n",tb.data);
					
				    ResponseGet res = deserializeResponseGet(tb.data);
				    //pthread_mutex_lock(&mutex_display);
				    printf("%s",res.msg);
				    printf("Grille :\n");
				    printOponentGrid(res.grid);
				    //pthread_mutex_unlock(&mutex_display);
				    
				    //lanceAttaque(serveur);
				    
				    //printf("\n\nChoisissez des coordonnées d'attaque: ");
				    printf("\nChoisissez des coordonnées d'attaque: \n");
				}else if(tb.data[0]== '1'){
				    ResponseAttack res = deserializeResponseAttack(tb.data);
				    //pthread_mutex_lock(&mutex_display);
				    printf("%s a attaqué.\nLe résultat est %s.\n", inet_ntoa(res.who), toString(res.result));
				    printf("Grille :\n");
				    printOponentGrid(res.grid);
				    printf("\nChoisissez des coordonnées d'attaque: \n");
				    
				    /*if (res.result !=WIN)
				        lanceAttaque(serveur);
				    else
				        printf("Grille terminée.\n\n");*/
				    //pthread_mutex_unlock(&mutex_display);
				}else{
					printf("%s",buffer);
				}
			}
		}else{
			perror("recv()");
			continue;
		}
	}
}

void byebye(void){
	printf("\nVous êtes désormais déconnecté(e).\n");
	if(send(serveur, "-", 2, 0) < 0){
		perror("send()");
		exit(-1);
	}
	close(serveur);
	close(sock_client);
}

void ctrlC_Handler(int e){
    exit(0);
}

int main(int argc, char **argv){
  
    hostent * hostinfo; 		/* info sur une machine hote */
    char * prog; 				/* nom du programme */
    char * host; 				/* nom de la machine distante */
  
  	atexit(byebye);
    signal(SIGINT, ctrlC_Handler);
      
    if (argc != 2) {
		perror("usage : ./client.exe <adresse-serveur>");
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

    printf("Numero de port pour la connexion au serveur : %d \n", ntohs(adresse_locale_globale.sin_port));
    
  	connexionAuServeur();
	
    exit(0);
}
