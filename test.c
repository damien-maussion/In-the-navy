
// gcc -std=gnu99 -pthread test.c grid.c -o test.exe

#include "grid.h"

TrameBuffer tb = {"",-1,0,false};
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
       	/*
		if (tb.finish){
			printf("%s\n", tb.data);
		}*/
    }
    idTrame++;
}

int main(){

	ResponseGet res;

	for (int i=0; i< GRID_WIDTH; i++){
		for (int j=0; j< GRID_HEIGHT; j++){
			res.grid[i][j] = i*j;
		}
	}

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
