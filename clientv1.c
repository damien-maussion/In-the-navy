/**
* \file client.c
* \brief Fichier d'implementation de la classe client
* \date 13/03/2015
* Commande :
* ./client.exe <adresse-serveur>
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#include "client.h"

/**
* \brief Demande une position d'attaque au client et l'envoie au serveur
* \param[in] sock Le socket auquel on envoie le message d'attaque (ici le serveur)
*/
void lanceAttaque(int sock){
	char pos[3];
	
	//on insere un caractere de reconnaissance de type de message, "1" signifie "attaque" pour le serveur
	char buffer[4] = "1";

	//en attente de saisie par l'utilisateur client
	scanf ("%3s",pos);  
	
	strcat(buffer, pos);

	//envoi des coordonnées d'attaque au serveur
	if(send(sock, buffer, strlen(buffer), 0) < 0){
		perror("send()");
		exit(-1);
	}
}

/**
* \brief Methode de connexion au serveur choisi par le client
*/
void connexionAuServeur(){

	if(sock_client != -1){
		close(sock_client);
		sock_client = -1;
	}
		
	//Creation de la socket serveur
    if ((serveur = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("erreur : impossible de creer la socket de connexion avec le serveur.");
		exit(1);
    }

	//Tentative de connexion au serveur dont les infos sont dans adresse_locale
    if ((connect(serveur, (sockaddr*)(&adresse_locale), sizeof(adresse_locale))) < 0) {
		perror("erreur : impossible de se connecter au serveur.");
		exit(1);
    }
    
    printf("Connexion au serveur établie. \n");
    
	tb.idTrame=-1;
    tb.finish=false;

	//Creation du thread d'ecoute pour le client
	pthread_t thread_listen;
    if (pthread_create(&thread_listen, NULL, ecoute, NULL)){
        perror("Impossible creer thread listen");
        exit(-1);
    }
    
    //Boucle infinie d'attaque
    while(1){
    	lanceAttaque(serveur);
    }
	
	if (pthread_join(thread_listen, NULL)){
        perror("Impossible joindre thread");
        exit(-1);
    }
}

/**
* \brief Methode du thread d'ecoute, le client attend de recevoir des messages
*/
void* ecoute(){

	//Buffer pour la reception des messages
    char buffer[TAILLE_MAX_TRAME];
    
    while(1){

		if (recv(serveur, buffer, sizeof(buffer), 0) >= 0){
		
			//"Decodage" de la trame recue
			Trame t = deserializeTrame(buffer);
			receveTrame(&tb, t);
			
			//Si toutes les trames ont ete recues, on traite le message dans son entierete
			if (tb.finish){
			
				if(tb.data[0] == '-'){					//code pour serveur hors-ligne
					printf("%s\n",buffer);
					exit(0);

				}else if (tb.data[0]== '0'){			//code pour arrivee sur le serveur
					
				    ResponseGet res = deserializeResponseGet(tb.data);
				    printf("%s",res.msg);
				    printf("Grille du serveur :\n");
				    printOponentGrid(res.grid);
				    printf("\nChoisissez des coordonnées d'attaque: \n");
				    
				}else if(tb.data[0]== '1'){				//code pour resultat d'attaque
				
				    ResponseAttack res = deserializeResponseAttack(tb.data);
				    printf("%s a attaqué en %s.\nLe résultat est : %s.\n", inet_ntoa(res.who), res.pos, toString(res.result));
				    printf("Grille du serveur :\n");
				    printOponentGrid(res.grid);
				    printf("\nChoisissez des coordonnées d'attaque: \n");

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

/**
* \brief Methode appelee lorsque le programme client se termine
*/
void byebye(void){
	printf("\nVous êtes désormais déconnecté(e).\n");
	
	//On previent le serveur que l'on quitte la partie
	if(send(serveur, "-", 2, 0) < 0){
		perror("send()");
		exit(-1);
	}
	
	//Fermeture des sockets
	close(serveur);
	close(sock_client);
}

/**
* \brief Redirection d'une fermeture de programme par la commande "Ctrl+c" vers la methode "byebye"
* \param[in] e
*/
void ctrlC_Handler(int e){
    exit(0);
}

/**
* \brief Main, creation de la connexion vers le serveur
* \param[in] argc
* \param[in] *argv adresse du serveur
* \return exit(0)
*/
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
    
    adresse_locale.sin_addr = *(in_addr *) hostinfo->h_addr;
	adresse_locale.sin_port = htons(5000);
	adresse_locale.sin_family = AF_INET;

    printf("Numero de port pour la connexion au serveur : %d \n", ntohs(adresse_locale.sin_port));
    
  	connexionAuServeur();
	
    exit(0);
}
