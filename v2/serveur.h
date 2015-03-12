#ifndef SERVEUR_H
#define SERVEUR_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <linux/types.h>
#include <unistd.h> /* close */
#include <netdb.h> /* gethostbyname */
#include "../grid.h" 
#include "../annexe.c" 

#define CRLF "\r\n"
#define PORT_SERVEUR 5000
#define MAX_CLIENTS 10

in_addr client;
bool isInGame = false;

static void app(void);
static int init_connection(void);
static void end_connection(int sock);
static int read_client(int sock, char *buffer);
static void write_client(int sock, const char *buffer);
void byebye(void);
void ctrlC_Handler(int e);

#endif

