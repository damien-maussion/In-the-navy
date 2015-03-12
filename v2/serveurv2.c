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
		int csock;
		
		if ((csock = accept(sock, (sockaddr *)&csin, &sinsize)) == -1){
            perror("erreur : impossible d'accepter la connexion avec le client.");
            continue;
        }else{
		    printf("Nouveau client hébergé.\n");

			//int longueur;
			char buff[50];
		   
			/*if ((longueur = read(csock, buff, sizeof(buff))) <= 0){
				continue;
			}*/
				
			if (!isInGame){
				isInGame=true;
				client = csin.sin_addr;
				if(send(csock, "fake", 4, 0) < 0)
				{
					perror("send()");
					exit(-1);
				}
			}
			else{
				if(send(csock, inet_ntoa(client), 50, 0) < 0)
				{
					perror("send()");
					exit(-1);
				}
				isInGame=false;
			}
		}
		
		/*char buffer[50];
		int n = 0;
		if((n = recv(csock, buffer, 49, 0)) < 0){
			perror("recv()");
			n = 0;
		}
   		buffer[n] = 0;
   		printf("%s\n",buffer);*/
		
		/* //after connecting the client sends its name 
		if(read_client(csock, buffer) == -1){
			continue;
		}*/
	}
	end_connection(sock);
}

static int init_connection(void)
{
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
	
	printf("numero de port pour la connexion au serveur : %d \n", ntohs(sin.sin_port));  
    printf("Lancement de l'écoute\n");
    
	return sock;
}

static void end_connection(int sock)
{
   close(sock);
}

static int read_client(int sock, char *buffer)
{
   int n = 0;

   /*if((n = recv(sock, buffer, BUF_SIZE - 1, 0)) < 0)
   {
      perror("recv()");
      n = 0;
   }

   buffer[n] = 0;*/

   return n;
}

static void write_client(int sock, const char *buffer)
{
   /*if(send(sock, buffer, strlen(buffer), 0) < 0)
   {
      perror("send()");
      exit(errno);
   }*/
}

void byebye(void){
	printf("Le serveur est désormais hors-ligne.\n");
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

