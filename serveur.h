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
#include <pthread.h>

#include "grid.h" 
//#include "annexe.c"

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

static int app(void);
static int init_connection(void);
static void end_connection(int sock);
static int read_client(int sock, char *buffer);
static void write_client(int sock, const char *buffer);
//static void send_message_to_all_clients(Client *clients, Client client, int actual, const char *buffer, char from_server);
void *connection_handler(void *socket_desc);
static void remove_client(int sock);
void ajout_client(int sock);
void clear_clients();
void diffusion(char* s);
void byebye(void);
void ctrlC_Handler(int e);

#endif

