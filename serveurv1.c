#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "serveur.h"
//#include "client.h"

static int app(void){

	int sock = init_connection();
	char buffer[BUF_SIZE];

	int nouv_client, *csock;
	struct sockaddr_in client;
	char *message;

	int c = sizeof(struct sockaddr_in);
    while((nouv_client = accept(sock, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        printf("Nouveau client hébergé.\n");
         
        //Reply to the client
        message = "Hello Client , I have received your connection. And now I will assign a handler for you\n";
        //write(new_socket , message , strlen(message));
        send(nouv_client, message, strlen(message), 0);
         
        pthread_t sniffer_thread;
        csock = malloc(1);
        *csock = nouv_client;
         
        if(pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) csock) < 0){
            perror("could not create thread");
            return -1;
        }
         
        //Now join the thread , so that we dont terminate before the thread
        //pthread_join( sniffer_thread , NULL);
        printf("Handler assigned\n");
    }
     
    if (nouv_client<0){
        perror("accept failed");
        return -1;
    }
}

void *connection_handler(void *socket_desc){

    //Get the socket descriptor
    int sock = *(int*)socket_desc;
    int read_size;
    char *message , client_message[2000];
     
    //Send some messages to the client
    message = "Greetings! I am your connection handler\n";
    //write(sock , message , strlen(message));
    send(sock, message, strlen(message), 0);
     
    message = "Now type something and i shall repeat what you type \n";
    //write(sock , message , strlen(message));
    send(sock, message, strlen(message), 0);
     
    //Receive a message from client
    while( (read_size = recv(sock , client_message , 2000 , 0)) > 0 ){
    
        //Send the message back to client
        //write(sock , client_message , strlen(client_message));
        send(sock, client_message, strlen(client_message), 0);
    }
     
    if(read_size == 0){
        puts("Client disconnected");
        fflush(stdout);
        remove_client(sock);
    }else if(read_size == -1){
        perror("recv failed");
    }
         
    //Free the socket pointer
    free(socket_desc);
     
    return 0;
}

void clear_clients(){

	Liste_clients *client_courant = clients;
	while (client_courant !=NULL){
		close(client_courant->socket);
		client_courant = client_courant->next;
	}
}

static void remove_client(int sock){

	if (clients){
        if (clients->socket == sock){
            Liste_clients *tmp = clients;
            clients = clients->next;
            close(tmp->socket);
            free(tmp);
        }else{

            Liste_clients *it = clients;
    
            while(it->next && clients->next->socket != sock){
                it = it->next;
            }

            if (it->next){
                Liste_clients *tmp = it->next;
                it->next = it->next->next;
                close(tmp->socket);
                free(tmp);
            }else{
                perror("erreur : ce client n'existe pas.");
            }
        }
    }
}

/*static void send_message_to_all_clients(Client *clients, Client sender, int actual, const char *buffer, char from_server){

	int i = 0;
	char message[BUF_SIZE];
	message[0] = 0;
	for(i = 0; i < actual; i++){
		//we don't send message to the sender
		if(sender.sock != clients[i].sock){
			if(from_server == 0)
			{
				strncpy(message, sender.name, BUF_SIZE - 1);
				strncat(message, " : ", sizeof message - strlen(message) - 1);
			}
			strncat(message, buffer, sizeof message - strlen(message) - 1);
			write_client(clients[i].sock, message);
		}
	}
}*/

static int init_connection(void){

	//Initialisation de la socket du serveur
	sockaddr_in sin = { 0 };

	if((serveur = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		perror("socket()");
		exit(-1);
	}

	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	sin.sin_port = htons(PORT_SERVEUR);
	sin.sin_family = AF_INET;

	if(bind(serveur,(sockaddr *) &sin, sizeof sin) == -1){
		perror("bind()");
		exit(-1);
	}

	if(listen(serveur, MAX_CLIENTS) == -1){
		perror("listen()");
		exit(-1);
	}
	
	//Création de la grille
    srand(time(NULL));
    init(&g);
    printGrid(g);

	//Initialisation du mutex
	/*if (pthread_mutex_init(&mutex_grid, NULL) != 0)
    {
        perror("\n mutex_grid init failed\n");
        return 1;
    }*/
	
	printf("Numero de port pour la connexion au serveur : %d \n", ntohs(sin.sin_port));  
    printf("Lancement de l'écoute\n");

	return serveur;
}

static void end_connection(int sock){

   close(sock);
}

static int read_client(int sock, char *buffer){

	/*int n = 0;

	if((n = recv(sock, buffer, BUF_SIZE - 1, 0)) < 0){
		perror("recv()");
		// if recv error we disonnect the client
		n = 0;
	}

	buffer[n] = 0;

	return n;*/
	return 0;
}

static void write_client(int sock, const char *buffer){

	/*if(send(sock, buffer, strlen(buffer), 0) < 0){
		perror("send()");
		exit(-1);
	}*/
}

void BroadCast(char* str){

	int offset = 0;
	int length = strlen(str);
    while (offset < length){
        Trame t;
        t.idTrame = idTrame;
        t.index = offset;
        t.taille = length;
        memcpy(t.data, str+offset, TAILLE_MAX_DATA_TRAME);
        offset+= TAILLE_MAX_DATA_TRAME;
		Liste_clients *client_courant = clients;
		int taille_trame = TAILLE_MAX_DATA_TRAME + 3*sizeof(int);
		while (client_courant !=NULL){

			if(send(client_courant->socket, t.data, taille_trame, 0) < 0){
				perror("send()");
				continue;
			}
			client_courant = client_courant->next;
		}
    }
    idTrame++;
}

void byebye(void){

	char buffer[100] = "*\nLe serveur est hors-ligne.";
	printf("%s\n",buffer);
	BroadCast(buffer);
	clear_clients();
	end_connection(serveur);
}

void ctrlC_Handler(int e){
    exit(0);
}

int main(int argc, char **argv){

	atexit(byebye);
    signal(SIGINT, ctrlC_Handler);
    
	app();

	return 0;
}

