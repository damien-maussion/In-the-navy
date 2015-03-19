#define GRID_WIDTH 10
#define GRID_HEIGHT 10

#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */

#include <stdio.h>
#include <stdlib.h>  
#include <time.h>
#include <string.h>
#include <stdbool.h>

#include <linux/types.h> 	/* pour les sockets */
#include <sys/socket.h>
#include <netdb.h>

#include <netinet/in.h>
#include <netinet/ip.h>

typedef struct in_addr in_addr;

// enum resultAttack
enum resultAttack {ERROR, REPEAT, WATER, TOUCH, SUNK, WIN};
typedef enum resultAttack resultAttack;

char * toString(resultAttack res);

// Position && PositionLetterDigit

struct PositionLetterDigit{
	char letter;
	int y;
};
typedef struct PositionLetterDigit PositionLetterDigit;

struct Position{
	int x,y;
};
typedef struct Position Position;

Position toPosition(PositionLetterDigit pld);

// Batiment

struct Batiment{
	int id,length;
};
typedef struct Batiment Batiment;

//Grid

struct Grid{
	int nbBoat;
	int grid[GRID_WIDTH][GRID_HEIGHT];
};
typedef struct Grid Grid;

/* Attention : 
batiment.length < GRID_WIDTH &&
batiment.length < GRID_HEIGHT

Sinon on a rand()%0
*/
void init(Grid *g);

int getGridStringLength();

char* getGrid(Grid g);
void printGrid(Grid g);

//char* getOponentGrid(Grid g);
//void printOponentGrid(Grid g);

void getOponentGrid(Grid g, int tab[GRID_WIDTH][GRID_HEIGHT]);
void printOponentGrid(int tab[GRID_WIDTH][GRID_HEIGHT]);



bool isValidPosition(Position p);

bool isOver(Grid g);
bool isLast(Grid g, Position p);

resultAttack attackPos(Grid *g, Position p);
resultAttack attack(Grid *g, PositionLetterDigit p);


#define TAILLE_MAX_DATA_TRAME 256
#define TAILLE_MAX_TRAME TAILLE_MAX_DATA_TRAME + 3 * sizeof(int)
#define TAILLE_RESPONSE 5000


struct ResponseGet{
	char msg[TAILLE_MAX_DATA_TRAME];
	int grid[GRID_WIDTH][GRID_HEIGHT];
};
typedef struct ResponseGet ResponseGet;

char* serializeResponseGet(ResponseGet r);
ResponseGet deserializeResponseGet(char* str);

struct ResponseAttack{
	resultAttack result;			//résultat de l'attaque
	int grid[GRID_WIDTH][GRID_HEIGHT];
	in_addr who;					//IP attaquant
	char pos[4];					//position de l'attaque
};
typedef struct ResponseAttack ResponseAttack;

char* serializeResponseAttack(ResponseAttack r);
ResponseAttack deserializeResponseAttack(char *str);


struct Trame{
    char data[TAILLE_MAX_DATA_TRAME];
    int idTrame;
    int index;
    int taille;
};
typedef struct Trame Trame;

char* serializeTrame(Trame t);
Trame deserializeTrame(char * str);

struct TrameBuffer{
	char data[2*TAILLE_RESPONSE];
	int idTrame;
	int nbTrameReceved;
	bool finish;
};
typedef struct TrameBuffer TrameBuffer;

void receveTrame(TrameBuffer *tbuf, Trame t);
