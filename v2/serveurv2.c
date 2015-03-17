#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "serveur.h"

static void app(void)
{
	int sock = init_connection();    
	
	while(1){

		sockaddr_in csin = { 0 };
		int sinsize = sizeof(csin);
		if(client != -1){
			//réception d'un ping ou message du client au cas où il se déconnecte
			read_client(client);
		}
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
				//envoi d'un message au client pour lui dire d'attendre un adversaire
				if(send(tmp, "fake", 4, 0) < 0){
					perror("send()");
					exit(-1);
				}
			}else{
				//envoi d'un ping au client
				write_client(client);
				//envoi de l'adresse d'un client à celui enregistré pour qu'ils communiquent
				if(send(tmp, inet_ntoa(adr_client), 50, 0) < 0){
					perror("send()");
					exit(-1);
				}
				isInGame=false;
			}
			client = tmp;
		}
	}
	end_connection(sock);
}

static int init_connection(void)
{
	//Initialisation de la socket du serveur
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	sockaddr_in sin = { 0 };

	if(sock == -1)
	{
	  perror("socket()");
	  exit(-1);
	}

	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	sin.sin_port = htons(PORT_SERVEUR);
	sin.sin_family = AF_INET;

	if(bind(sock,(sockaddr *) &sin, sizeof sin) == -1)
	{
	  perror("bind()");
	  exit(-1);
	}

	if(listen(sock, MAX_CLIENTS) == -1)
	{
	  perror("listen()");
	  exit(-1);
	}
	
	printf("Numero de port pour la connexion au serveur : %d \n", ntohs(sin.sin_port));  
    printf("Lancement de l'écoute\n");
    
	return sock;
}

static void end_connection(int sock)
{
   close(sock);
}

static int read_client(int sock)
{
	int n = 0;
	char buffer[10];
	if((n = recv(sock, buffer, 10, 0)) < 0){
		perror("recv()");
		n = 0;
	}
	if(buffer[0] == '-'){
		//réception d'un message de déconnexion de la part du client
		close(client);
		client = -1;
		printf("Client déconnecté.\n");
		isInGame=false;
	}
	
	return n;
}

static void write_client(int sock)
{
	char* buffer = "ping";
	if(sock != -1){
		//envoi d'un ping au client
		if(send(sock, buffer, strlen(buffer), 0) < 0){
			perror("send()");
			exit(-1);
		}
	}
}

void byebye(void){
	char buffer[100] = "*\nLe serveur est hors-ligne.";
	printf("%s\n",buffer);
	if(client != -1){
		//envoi d'un message au client pour prévenir que le serveur est hors-ligne
		if(send(client, buffer, 100, 0) < 0){
			perror("send()");
			exit(-1);
		}
		close(client);
	}
}

void ctrlC_Handler(int e){
    exit(0);
}

int main(int argc, char **argv)
{
    atexit(byebye);
    signal(SIGINT, ctrlC_Handler);
    
	app();
	return EXIT_SUCCESS;
}

