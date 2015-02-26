
// gcc -std=gnu99 -pthread test.c grid.c -o test.exe

#include "grid.h"

TrameBuffer tb = {NULL,-1,0,false};
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
		if (tb.finish){
			printf("%s\n", tb.data);
		}
    }
    idTrame++;
}

int main(){


	char *str2= (char*) malloc(1500* sizeof(char));
	if (str2==NULL) exit (1);
	for (int i=0;i<1499;i++)
		strcat(str2, "c");
	str2[1499]='\0';
	test(str2, strlen(str2));
	free(tb.data);
	free(str2);

	printf("1\n");
	char *str3= (char*) malloc(1500* sizeof(char));
	if (str3==NULL) exit (1);
	printf("2\n");
	for (int i=0;i<1499;i++)
		str3[i]='d';
	str3[1499]='\0';
	test(str3, strlen(str3));
	free(tb.data);
	free(str3);

	char str[1500]="";
	for (int i=0;i<1500;i++)
		strcat(str, "a");
	test(str, strlen(str));
	free(tb.data);

	for (int i=0;i<1500;i++)
		str[i] = 'b';
	test(str, strlen(str));
	free(tb.data);

	str2= malloc(1500* sizeof(char));
	for (int i=0;i<1500;i++)
		strcat(str2, "d");
	test(str2, strlen(str2));
	free(tb.data);
	free(str2);
	
	return 0;
}