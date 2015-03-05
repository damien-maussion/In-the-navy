
// gcc -std=gnu99 -pthread test.c grid.c -o test.exe

#include "grid.h"

TrameBuffer tb;
int idTrame=0;

void test(char * str, int length){
    int offset = 0;

    //printf("resp : %d - %d\n%s\n", (int) strlen(str), getGridStringLength(), str);

    while (offset < length){
        Trame t;
        t.idTrame = idTrame;
        t.index = offset;
        t.taille = length;
        memcpy(t.data, str+offset, TAILLE_MAX_DATA_TRAME);
        offset+= TAILLE_MAX_DATA_TRAME;
        //write(sd,serializeTrame(t),TAILLE_MAX_DATA_TRAME + 3*sizeof(int));
       	receveTrame(&tb, t);
       	printf("test : idTrame : %d, nbTrameReceved: %d, finish: %d\n", tb.idTrame, tb.nbTrameReceved, tb.finish);

       	/*
		if (tb.finish){
			printf("%s\n", tb.data);
		}*/
    }
    idTrame++;
}

struct A{
	int a;
};
typedef struct A A;
void f(A *a){
	a->a++;
}
A a;

int main(){

	Grid g;
	init(&g);

	//a = (A*) malloc(sizeof(A));
	a.a=0;

	//tb = (TrameBuffer*) malloc(sizeof(TrameBuffer));
	tb.idTrame = -1;

	for (int i=1; i<=10; i++){
		f(&a);
		
		PositionLetterDigit pos = {letter: 'A', y: i};
		attack(&g,pos);

		ResponseGet res;
		getOponentGrid(g, res.grid);

		test(serializeResponseGet(res), TAILLE_RESPONSE);

		printf("main : idTrame : %d, nbTrameReceved: %d, finish: %d\n", tb.idTrame, tb.nbTrameReceved, tb.finish);


		ResponseGet res2;
		res2 = deserializeResponseGet(tb.data);

		printf("%d\n", tb.data[0]);
		printOponentGrid(res2.grid);
		
	}
	printf("a : %d\n", a.a);
	//free(a);
/*
	for (int i=0; i< GRID_WIDTH; i++){
		for (int j=0; j< GRID_HEIGHT; j++){
			res.grid[i][j] = i*j;
		}
	}
*/
/*
	for (int i=0; i< GRID_WIDTH; i++){
		for (int j=0; j< GRID_HEIGHT; j++){
			printf("%02d ", res.grid[i][j]);
		}
		printf("\n");
	}

	char *str = serializeResponseGet(res);
	printf("ha : %s\n", str);

	//ResponseGet res2 = deserializeResponseGet(str);

	test(str, TAILLE_RESPONSE);
	ResponseGet res2 = deserializeResponseGet(tb.data);
	
	for (int i=0; i< GRID_WIDTH; i++){
		for (int j=0; j< GRID_HEIGHT; j++){
			printf("%02d ", res2.grid[i][j]);
		}
		printf("\n");
	}


	test(str, TAILLE_RESPONSE);
	res2 = deserializeResponseGet(tb.data);
	
	for (int i=0; i< GRID_WIDTH; i++){
		for (int j=0; j< GRID_HEIGHT; j++){
			printf("%02d ", res2.grid[i][j]);
		}
		printf("\n");
	}


/*
	char *str2= (char*) malloc(1500* sizeof(char));
	if (str2==NULL) exit (1);
	for (int i=0;i<1499;i++)
		strcat(str2, "c");
	str2[1499]='\0';
	test(str2, strlen(str2));
	//free(tb.data);
	free(str2);

	str2= (char*) malloc(1500* sizeof(char));
	if (str2==NULL) exit (1);
	for (int i=0;i<1499;i++)
		strcat(str2, "c");
	str2[1499]='\0';
	test(str2, strlen(str2));
	//free(tb.data);
	free(str2);

str2= (char*) malloc(1500* sizeof(char));
	if (str2==NULL) exit (1);
	for (int i=0;i<1499;i++)
		strcat(str2, "c");
	str2[1499]='\0';
	test(str2, strlen(str2));
	//free(tb.data);
	free(str2);

str2= (char*) malloc(1500* sizeof(char));
	if (str2==NULL) exit (1);
	for (int i=0;i<1499;i++)
		strcat(str2, "c");
	str2[1499]='\0';
	test(str2, strlen(str2));
	//free(tb.data);
	free(str2);

str2= (char*) malloc(1500* sizeof(char));
	if (str2==NULL) exit (1);
	for (int i=0;i<1499;i++)
		strcat(str2, "c");
	str2[1499]='\0';
	test(str2, strlen(str2));
	//free(tb.data);
	free(str2);

str2= (char*) malloc(1500* sizeof(char));
	if (str2==NULL) exit (1);
	for (int i=0;i<1499;i++)
		strcat(str2, "c");
	str2[1499]='\0';
	test(str2, strlen(str2));
	//free(tb.data);
	free(str2);

str2= (char*) malloc(1500* sizeof(char));
	if (str2==NULL) exit (1);
	for (int i=0;i<1499;i++)
		strcat(str2, "c");
	str2[1499]='\0';
	test(str2, strlen(str2));
	//free(tb.data);
	free(str2);

str2= (char*) malloc(1500* sizeof(char));
	if (str2==NULL) exit (1);
	for (int i=0;i<1499;i++)
		strcat(str2, "c");
	str2[1499]='\0';
	test(str2, strlen(str2));
	//free(tb.data);
	free(str2);

str2= (char*) malloc(1500* sizeof(char));
	if (str2==NULL) exit (1);
	for (int i=0;i<1499;i++)
		strcat(str2, "c");
	str2[1499]='\0';
	test(str2, strlen(str2));
	//free(tb.data);
	free(str2);

str2= (char*) malloc(1500* sizeof(char));
	if (str2==NULL) exit (1);
	for (int i=0;i<1499;i++)
		strcat(str2, "c");
	str2[1499]='\0';
	test(str2, strlen(str2));
	//free(tb.data);
	free(str2);

str2= (char*) malloc(1500* sizeof(char));
	if (str2==NULL) exit (1);
	for (int i=0;i<1499;i++)
		strcat(str2, "c");
	str2[1499]='\0';
	test(str2, strlen(str2));
	//free(tb.data);
	free(str2);

str2= (char*) malloc(1500* sizeof(char));
	if (str2==NULL) exit (1);
	for (int i=0;i<1499;i++)
		strcat(str2, "c");
	str2[1499]='\0';
	test(str2, strlen(str2));
	//free(tb.data);
	free(str2);
	
	return 0;
	*/
}
