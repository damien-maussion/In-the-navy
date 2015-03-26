/**
* \file serveur.c
* \brief Fichier d'implementation de la classe serveur
* \date 13/03/2015
* Commande :
* ./serveur.exe
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "serveur.h"

/**
* \brief Initialisation de la connexion
* \return Socket du serveur
*/
int init_connexion(void){

	//Initialisation de la socket du serveur
	int sock;
	sockaddr_in sin = { 0 };

	if((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1){
	  perror("socket()");
	  exit(-1);
	}

	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	sin.sin_port = htons(PORT_SERVEUR);
	sin.sin_family = AF_INET;

	if(bind(sock,(sockaddr *) &sin, sizeof sin) == -1){
	  perror("bind()");
	  exit(-1);
	}

	if(listen(sock, MAX_CLIENTS) == -1){
	  perror("listen()");
	  exit(-1);
	}
	
	printf("Numero de port pour la connexion au serveur : %d \n", ntohs(sin.sin_port));  
    printf("Lancement de l'écoute\n");
    
	return sock;
}

/**
* \brief Methode appelee lorsque le programme client se termine
* \param[in] sock Socket du client duquel on recoit un message
*/
int lire_client(int sock){

	int n = 0;
	char buffer[10];
	//Reception d'un message de la part du client
	if((n = recv(sock, buffer, 10, 0)) < 0){
		perror("recv()");
		n = 0;
	}
	if(buffer[0] == '-'){		//code pour deconnexion du client
		close(client);
		client = -1;
		printf("Client déconnecté.\n");
		isInGame=false;
	}
	
	return n;
}

/**
* \brief Methode appelee lorsque le programme client se termine
* \param[in] sock Socket du client a qui on envoie le ping
*/
void ecrire_client(int sock){

	char* buffer = "ping";
	if(sock != -1){
		//Envoi d'un ping au client
		if(send(sock, buffer, strlen(buffer), 0) < 0){
			perror("send()");
			exit(-1);
		}
	}
}

/**
* \brief Methode appelee lorsque le programme client se termine
*/
void byebye(void){

	char buffer[100] = "*\nLe serveur est hors-ligne.";
	printf("%s\n",buffer);
	if(client != -1){
		//Envoi d'un message au client pour prévenir que le serveur est hors-ligne
		if(send(client, buffer, 100, 0) < 0){
			perror("send()");
			exit(-1);
		}
		close(client);
	}
}

/**
* \brief Redirection d'une fermeture de programme par la commande "Ctrl+c" vers la methode "byebye"
* \param[in] e
*/
void ctrlC_Handler(int e){
    exit(0);
}

/**
* \brief Main, creation de la connexion
* \param[in] argc
* \param[in] *argv
* \return exit(0)
*/
int main(int argc, char **argv){

    atexit(byebye);
    signal(SIGINT, ctrlC_Handler);
    
	int sock = init_connexion();    
	
	while(1){

		sockaddr_in csin = { 0 };
		int sinsize = sizeof(csin);
		/*if(client != -1){
			//Reception d'un ping ou message du client dans le cas où il se déconnecte
			lire_client(client);
		}*/
		int tmp;
		if((tmp = accept(sock, (sockaddr *)&csin, &sinsize)) == -1){
            perror("erreur : impossible d'accepter la connexion avec le client.");
            continue;
        }else{
		    printf("Nouveau client hébergé.\n");

			char buff[50];
				
			if (!isInGame){
				isInGame=true;
				adr_client = csin.sin_addr;
				//Envoi d'un message au client pour lui dire d'attendre un adversaire
				if(send(tmp, "fake", 4, 0) < 0){
					perror("send()");
					exit(-1);
				}
			}else{
				//Envoi d'un ping au client
				ecrire_client(client);
				//Envoi de l'adresse d'un client à celui enregistré pour qu'ils communiquent
				if(send(tmp, inet_ntoa(adr_client), 50, 0) < 0){
					perror("send()");
					exit(-1);
				}
				isInGame=false;
			}
			client = tmp;
		}
	}
	close(sock);
	
	return 0;
}

