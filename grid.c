
#include "grid.h"

// enum resultAttack

char * toString(resultAttack res){
	switch (res){
		case ERROR: 
			return "wrong position\n";
		case REPEAT:
			return "already done\n";
		case WATER:
			return "missed\n";
		case TOUCH:
			return "touch\n";
		case SUNK:
			return "sunk\n";
		case WIN:
			return "Win\n";
	}
	return "";
}

// Position && PositionLetterDigit

Position toPosition(PositionLetterDigit pld){
	Position p;
	p.x = pld.letter-'A';
	p.y = pld.y-1;
	return p;
}

//Grid

/* Attention : 
batiment.length < GRID_WIDTH &&
batiment.length < GRID_HEIGHT

Sinon on a rand()%0
*/
void init(Grid *g){

	Batiment bats[5];
	bats[0].id = 2;
	bats[0].length=5;
	bats[1].id=3;
	bats[1].length=4;
	bats[2].id=4;
	bats[2].length=3;
	bats[3].id=5;
	bats[3].length=3;
	bats[4].id=6;
	bats[4].length=2;
	
	int i,j;
	for (i=0; i<GRID_WIDTH;i++){
		for (j=0; j<GRID_HEIGHT;j++){
			g->grid[i][j]=0;
		}
	}
	
	int l =5;
	int t=0;

	for (i=0;i<l;i++){
		bool place=false;
		while(!place && t<10){
			
			int maxX = GRID_WIDTH;
			int maxY = GRID_HEIGHT;
			bool horizontal = rand()%2==0;

			if (horizontal)
				maxX -= bats[i].length;
			else
				maxY -= bats[i].length;

			Position p;
			p.x = rand()%maxX;
			p.y = rand()%maxY;

			if (horizontal){
				bool isOccuped=false;
				for (j=0;j<bats[i].length;j++){
					isOccuped = isOccuped || g->grid[p.x+j][p.y] !=0;
				}

				if (!isOccuped){
					for (j=0;j<bats[i].length;j++){
						g->grid[p.x+j][p.y] = bats[i].id;
					}
					place=true;
				}
			}
			else{
				bool isOccuped=false;
				for (j=0;j<bats[i].length;j++){
					isOccuped = isOccuped || g->grid[p.x][p.y+j] !=0;
				}

				if (!isOccuped){
					for (j=0;j<bats[i].length;j++){
						g->grid[p.x][p.y+j] = bats[i].id;
					}
					place=true;
				}
			}
		}

	}
	g->nbBoat=l;

}

/*
void print(Grid g){
	int i,j;
	printf("\n   |");
	for (j =0;j<GRID_WIDTH;j++){
		printf("%c ",'A'+j);
	}
	printf("\n----");
	for (j =0;j<GRID_WIDTH;j++){
		printf("--");
	}
	printf("\n");

	for (i=0;i<GRID_HEIGHT;i++){
		printf("%d%s |", (i+1), ((i+1)<10?" ":""));
		//std::cout <<(i+1)<< ((i+1)<10?" ":"") <<" |";
		for (j =0;j<GRID_WIDTH;j++){
			//std::cout << grid[j][i]<<" ";
			if (g.grid[j][i]==-1){
				printf("%s%c%s ", CYAN, 'F', RESET);
			}
			else if (g.grid[j][i] <-1){
				printf("%s%d%s ", RED, (-1*g.grid[j][i]), RESET);
			}
			else{
				printf("%d ",g.grid[j][i]);
			}
		}
		printf("\n");
	}
}*/

int getGridStringLength(){
	int nb_rows_display = GRID_HEIGHT+2;
	int max_row_size = 4 +(2 + 10)*GRID_WIDTH +1;  // 4 : "i  |", 2: "0 ", 10 : 2*strlen(CYAN), 1: "\n"
	return ( max_row_size * nb_rows_display) +1;
}

char* getGrid(Grid g){

	int str_length = getGridStringLength();
	char *str = malloc(str_length * sizeof(char));
	strcat(str, "   |");

	int i,j;
	for (j =0;j<GRID_WIDTH;j++){
		char c[2];
		snprintf(c, 2,"%c",('A'+j));
		strcat(str,c);
		strcat(str," ");
	}

	strcat(str,"\n----");
	for (j =0;j<GRID_WIDTH;j++){
		strcat(str, "--");
	}
	strcat(str, "\n");

	for (i=0;i<GRID_HEIGHT;i++){
		char dig[3];
		snprintf(dig, 3,"%d",(i+1));

		strcat(str, dig);
		strcat(str, ((i+1)<10?"  |":" |"));
		for (j =0;j<GRID_WIDTH;j++){
			if (g.grid[j][i]==-1){
				strcat(str, CYAN);
				strcat(str, "F ");
				strcat(str, RESET);
			}
			else if (g.grid[j][i]<-1){
				snprintf(dig, 2,"%d",(-1*g.grid[j][i]));
				strcat(str, RED);
				strcat(str, dig);
				strcat(str, " ");
				strcat(str, RESET);
			}
			else{
				snprintf(dig, 2,"%d",g.grid[j][i]);
				strcat(str, dig);
				strcat(str, " ");
			}
		}
		strcat(str, "\n");
	}
	strcat(str, "\n\0");

	return str;
}

void printGrid(Grid g){
	printf("%s\n", getGrid(g));
}

char* getOponentGrid(Grid g){

	int str_length = getGridStringLength();
	char *str = malloc(str_length * sizeof(char));
	strcat(str, "   |");

	int i,j;
	for (j =0;j<GRID_WIDTH;j++){
		char c[2];
		snprintf(c, 2,"%c",('A'+j));
		strcat(str,c);
		strcat(str," ");
	}

	strcat(str,"\n----");
	for (j =0;j<GRID_WIDTH;j++){
		strcat(str, "--");
	}
	strcat(str, "\n");

	for (i=0;i<GRID_HEIGHT;i++){
		char dig[3];
		snprintf(dig, 3,"%d",(i+1));

		strcat(str, dig);
		strcat(str, ((i+1)<10?"  |":" |"));
		for (j =0;j<GRID_WIDTH;j++){
			if (g.grid[j][i]==-1){
				strcat(str, CYAN);
				strcat(str, "F ");
				strcat(str, RESET);
			}
			else if (g.grid[j][i]<-1){
				strcat(str, RED);
				strcat(str, "X ");
				strcat(str, RESET);
			}
			else{
				strcat(str, "O ");
			}
		}
		strcat(str, "\n");
	}

	return str;
}

void printOponentGrid(Grid g){
	printf("%s\n", getOponentGrid(g));
}

bool isValidPosition(Position p){
	return p.x>=0 && p.y>=0 && p.x<GRID_WIDTH && p.y<GRID_HEIGHT;
}

bool isOver(Grid g){
	return g.nbBoat==0;
}
bool isLast(Grid g, Position p){
	for (int i=0;i<GRID_WIDTH;i++){
		for (int j =0;j<GRID_HEIGHT;j++){
			if (g.grid[i][j] == g.grid[p.x][p.y] && (i!=p.x || j!=p.y))
				return false;
		}
	}
	return true;
}

resultAttack attackPos(Grid *g, Position p){
	if (isOver(*g))
		return WIN;

	if (!isValidPosition(p))
		return ERROR;

	if (g->grid[p.x][p.y]<0)
		return REPEAT;

	if (g->grid[p.x][p.y] == 0){
		g->grid[p.x][p.y] = -1;
		return WATER;
	}
	
	if (g->grid[p.x][p.y] > 0){
		if (isLast(*g, p)){
			g->nbBoat--;
			g->grid[p.x][p.y] *= -1;

			if (g->nbBoat==0)
				return WIN;
			return SUNK;
		}
		g->grid[p.x][p.y] *= -1;
		return TOUCH;
	}
}
resultAttack attack(Grid *g, PositionLetterDigit p){
	return attackPos(g, toPosition(p));
}

/*
int main(){
	srand(time(NULL));

	PositionLetterDigit pld = {letter:'A', y:4};
	Position p = toPosition(pld);
	printf("%d-%d\n", p.x, p.y);


	Grid g;
	init(&g);
	printGrid(g);
	printOponentGrid(g);
	
	return 0;
}
*/

char* serializeTrame(Trame t){
    char * str = malloc( TAILLE_MAX_DATA_TRAME * sizeof(char) + 3 * sizeof(int));
    
    memcpy(str, t.data, TAILLE_MAX_DATA_TRAME * sizeof(char));
    int offset=TAILLE_MAX_DATA_TRAME;
    
    memcpy(str+offset, &(t.idTrame), sizeof(int));
    offset+=sizeof(int);

    memcpy(str+offset, &(t.index), sizeof(int));
    offset+=sizeof(int);

    memcpy(str+offset, &(t.taille), sizeof(int));
    offset+=sizeof(int);

    return str;
}

Trame deserializeTrame(char * str){
    Trame t;

    memcpy(&(t.data), str, TAILLE_MAX_DATA_TRAME* sizeof(char));
    memcpy(&(t.idTrame), str+ TAILLE_MAX_DATA_TRAME, sizeof(int));
    memcpy(&(t.index), str + TAILLE_MAX_DATA_TRAME + sizeof(int), sizeof(int));
    memcpy(&(t.taille), str + TAILLE_MAX_DATA_TRAME + 2 * sizeof(int), sizeof(int));

    return t;
}

void receveTrame(TrameBuffer *tb, Trame t){
	printf("trame %d-%d\n", t.idTrame, t.index);
	if (tb->idTrame < t.idTrame){
		//free(tb->data);
		printf("taille : %d\n", t.taille);
		tb->data = malloc( t.taille *sizeof(char));
		tb->idTrame = t.idTrame;
		tb->nbTrameReceved = 0;
		tb->finish = false;
	}
	
	memcpy( &(tb->data[t.index]), &(t.data), TAILLE_MAX_DATA_TRAME*sizeof(char));
	tb->nbTrameReceved++;
	if (tb->nbTrameReceved*TAILLE_MAX_DATA_TRAME >= t.taille)
		tb->finish = true;
}