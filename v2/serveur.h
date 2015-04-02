#ifndef SERVEUR_H
#define SERVEUR_H

/**
* \file serveur.h
* \brief Fichier de declaration de la classe serveur
* \date 13/03/2015
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <linux/types.h>
#include <unistd.h> /* close */
#include <netdb.h> /* gethostbyname */
#include <pthread.h>

#include "grid.h"

typedef struct sockaddr_in sockaddr_in;
typedef struct sockaddr sockaddr;
typedef struct in_addr in_addr;
typedef struct hostent hostent;
typedef struct servent servent;

typedef struct s_List Liste_clients;
struct s_List{

    Liste_clients *next;
    int socket;
};

#define PORT_SERVEUR 5000
#define MAX_CLIENTS 10

#define BUF_SIZE 1024

Grid g;
Liste_clients *clients = NULL;
int serveur;
int idTrame = 0;

int init_connexion(void);
void *prise_en_charge_client(void *socket_desc);
void envoi_trame(int sock, char* msg, int length);
void ajout_client(int sock);
void suppr_client(int sock);
void vide_clients();
void diffusion(char* s, int length);
void byebye(void);
void ctrlC_Handler(int e);

#endif

