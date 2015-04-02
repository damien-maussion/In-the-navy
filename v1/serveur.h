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

#include "grid.h"

typedef struct sockaddr 	sockaddr;
typedef struct sockaddr_in 	sockaddr_in;
typedef struct hostent 		hostent;
typedef struct servent 		servent;

#define PORT_SERVEUR 5000
#define MAX_CLIENTS 10

in_addr adr_client;
int socket_premier_client = -1;
bool isInGame = false;

int init_connexion(void);
static int lire_client(int sock);
static void ecrire_client(int sock);
void byebye(void);
void ctrlC_Handler(int e);

#endif

