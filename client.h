#ifndef CLIENT_H
#define CLIENT_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <linux/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <strings.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <pthread.h>

#include "grid.h"
//#include "annexe.c"

typedef struct sockaddr 	sockaddr;
typedef struct sockaddr_in 	sockaddr_in;
typedef struct hostent 		hostent;
typedef struct servent 		servent;

#define TAILLE_MAX_NOM 256
#define PORT_CLIENT 5001

sockaddr_in adresse_locale_globale; /* adresse de socket local */

bool partieEnCours = false;
int serveur = -1;
int sock_client = -1;
TrameBuffer tb;

//void reinitOponentGrid();
//void updateOpGrid(PositionLetterDigit p, resultAttack res);
void lanceAttaque(int sock);
void attente(int csock);
void connexionAuServeur();
void* ecoute();
void byebye(void);
void ctrlC_Handler(int e);

#endif
