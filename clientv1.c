/*-----------------------------------------------------------
Client a lancer apres le serveur avec la commande :
./client.exe <adresse-serveur>
------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#include "client.h"

/*void reinitOponentGrid(){
	//réinitialisation de la grille
	for (int i=0; i<GRID_WIDTH;i++){
		for (int j=0; j<GRID_HEIGHT;j++){
			opGrid[i][j]=0;
		}
	}
}*/

/*void updateOpGrid(PositionLetterDigit p, resultAttack res){
	//mise-à-jour de la grille adverse après attaque
	Position pos= toPosition(p);
	if (res == WATER)
		opGrid[pos.x][pos.y]=-1;
	else if (res==TOUCH || res == SUNK || res == WIN)
		opGrid[pos.x][pos.y]=-2;
}*/

void lanceAttaque(int sock){
	char pos[3];
	char buffer[4] = "1";

	printf ("Choisissez des coordonnées d'attaque: ");
	scanf ("%3s",pos);  
	
	strcat(buffer, pos);

	//envoi des coordonnées d'attaque à l'adversaire
	if(send(sock, buffer, strlen(buffer), 0) < 0){
		perror("send()");
		exit(-1);
	}
	
	//en attente d'une réponse
	/*if(partieEnCours)
		attente(sock);
	else
		printf("err: partie terminée\n");*/
}

/*void attente(int csock){
	char buffer[100];
	char buf[100] = "0";
	int n = 0;

	//réception d'un message de l'adversaire
	if((n = recv(csock, buffer, sizeof(buffer), 0)) < 0){
		perror("recv()");
		exit(-1);
	}

	if(buffer[0] == '-'){				//déconnexion
		printf("%s\n",buffer);
		printf("Déconnexion de votre adversaire. Reconnexion avec le serveur de jeu en cours...\n");
		partieEnCours = false;
		adversaire = -1;
		//reconnexion au serveur
		connexionAuServeur();
	}else if (buffer[0]== '0'){			//réception d'un resultat

		//Traitement du résultat
    	resultAttack resA;
    	memcpy(&resA, buffer+sizeof(char), sizeof(resultAttack));
    	
    	PositionLetterDigit p;

		memcpy( &p.letter, buffer+sizeof(char)+sizeof(resultAttack), sizeof(char) );

		char subbuff[3];
		memcpy( subbuff, buffer+sizeof(resultAttack)+2*sizeof(char), 2 );
		subbuff[2] = '\0';
		p.y = atoi(subbuff);

    	printf("Attaque en %c%d, resultat %s\n", p.letter, p.y, toString(resA));

    	if (resA != REPEAT && resA!= ERROR){
			
			//Résultat valide; prise en compte du résultat
    		updateOpGrid(p, resA);
		
    		printf("Grille de l'adversaire :\n");
    		printOponentGrid(opGrid);

			if (resA==WIN){
				printf("Vous avez gagné.\n");
				partieEnCours = false;
				printf("Partie terminée. Reconnexion avec le serveur de jeu en cours...\n");
				close(adversaire);
				adversaire = -1;
				//reconnexion au serveur
				connexionAuServeur();
			}

			printf("Attentez que votre adversaire joue.\n");
			attente(csock);
		}else{
			//Résultat invalide, rejouer
			printf("Attaque incorrecte, reessayez.\n");
			lanceAttaque(csock);	
		}
    	
    }else if(buffer[0] == '1'){			//réception d'une attaque
	    
	    //Traitement de l'attaque
		PositionLetterDigit p;
		p.letter = buffer[1];
        char subbuff[3];
        memcpy(subbuff, &buffer[2], 2);
        subbuff[2] = '\0';
        p.y = atoi(subbuff);
        
        resultAttack resA= attack(&grid, p);
        printf("Attaqué en %c%d, résultat : %s\n", p.letter, p.y, toString(resA));
        
        char res[10];
		res[0]= '0';
		memcpy(res+sizeof(char), &resA, sizeof(resultAttack));
		memcpy(res+sizeof(char)+sizeof(resultAttack), buffer+sizeof(char), 3*sizeof(char));
        
        //Envoi du résultat de l'attaque à l'attaquant
        if(send(csock, res, 10, 0) < 0){
			perror("send()");
			exit(-1);
		}
		
        if (resA != REPEAT && resA!= ERROR){
			if (resA==WIN){
				printf("Vous avez perdu.\n");
				partieEnCours = false;
				printf("Partie terminée. Reconnexion avec le serveur de jeu en cours...\n");
				close(adversaire);
				adversaire = -1;
				//reconnexion au serveur
				connexionAuServeur();
			}
			printf("Votre grille :\n");
			printGrid(grid);
			//A vous d'attaquer
			lanceAttaque(csock);
		}else{
			attente(csock);
		}
	}else{								//erreur
		perror("erreur : données incomprises");
	}
}*/

void connexionAuServeur(){

	if(sock_client != -1){
		close(sock_client);
		sock_client = -1;
	}
		
	//Creation de la socket
    if ((serveur = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("erreur : impossible de creer la socket de connexion avec le serveur.");
		exit(1);
    }

	//Tentative de connexion au serveur dont les infos sont dans adresse_locale_globale
    if ((connect(serveur, (sockaddr*)(&adresse_locale_globale), sizeof(adresse_locale_globale))) < 0) {
		perror("erreur : impossible de se connecter au serveur.");
		exit(1);
    }
    
    //Envoi d'un ping au serveur
    /*if(send(serveur, "ping", 2, 0) < 0){
		perror("send()");
		exit(-1);
	}*/
    
    /*srand(time(NULL));
    init(&grid);
    reinitOponentGrid();*/
    
    printf("Connexion au serveur établie. \n");
    
    //memset(adr_adversaire, 0, sizeof(adr_adversaire));
    int n = 0;
    char bu[100];
	if((n = recv(serveur, bu, 100, 0)) < 0){
		perror("recv()");
		exit(-1);
	}
	printf("%s\n",bu);
	if((n = recv(serveur, bu, 100, 0)) < 0){
		perror("recv()");
		exit(-1);
	}
	printf("%s\n",bu);
	
	while(1){
		lanceAttaque(serveur);
	}

    /*if (strcmp(adr_adversaire, "fake")==0){
    	//Client devient serveur
    	sock_client = socket(AF_INET, SOCK_STREAM, 0);
		if(sock_client == -1){
			perror("socket()");
			exit(-1);
		}
		sockaddr_in sin = { 0 };
		sin.sin_addr.s_addr = htonl(INADDR_ANY);
		sin.sin_family = AF_INET;
		sin.sin_port = htons(PORT_CLIENT);
		if(bind (sock_client, (sockaddr *) &sin, sizeof sin) == -1){
			perror("bind()");
			exit(-1);
		}
		if(listen(sock_client, 1) == -1){
			perror("listen()");
			exit(-1);
		}
		
		sockaddr_in csin = { 0 };

		int sinsize = sizeof csin;
		printf("Recherche d'un adversaire en cours...\n");
		
		char buf[100];
		if((n = recv(serveur, buf, 100, 0)) < 0){
			perror("recv()");
			exit(-1);
		}
		if(buf[0] == '*'){
			printf("%s\n",buf);
			partieEnCours = false;
			exit(0);
		}

		if((adversaire = accept(sock_client, (sockaddr *)&csin, &sinsize)) == -1){
			perror("accept()");
			exit(-1);
		}else{
			printf("Adversaire trouvé.\n");
			partieEnCours = true;
			close(serveur);
			
			printf("Votre grille :\n");
    		printGrid(grid);
    		printf("Grille de l'adversaire :\n");
    		printOponentGrid(opGrid);
    		printf("Attendez que votre adversaire joue.\n");
			attente(adversaire);
		}
		
    }else{
    	//Nouveau socket, client du joueur 1 (serveur)
		sock_client = socket(AF_INET, SOCK_STREAM, 0);
		if(sock_client == -1){
			perror("socket()");
			exit(-1);
		}
		struct hostent *hostinfo = NULL;
		sockaddr_in sin = {0};

		hostinfo = gethostbyname(adr_adversaire);
		if (hostinfo == NULL){
			fprintf (stderr, "Hôte inconnu %s.\n", adr_adversaire);
			exit(-1);
		}

		inet_aton(adr_adversaire, &sin.sin_addr);
		sin.sin_port = htons(PORT_CLIENT);
		sin.sin_family = AF_INET;

		if(connect(sock_client,(sockaddr *) &sin, sizeof(sockaddr)) == -1){
			perror("connect()");
			exit(-1);
		}else{
			printf("Connexion au client %s, port : %d\n", inet_ntoa(sin.sin_addr),ntohs(sin.sin_port));
			partieEnCours = true;
			adversaire = sock_client;
			close(serveur);
			
			printf("Votre grille :\n");
    		printGrid(grid);
			printf("Grille de l'adversaire :\n");
			printOponentGrid(opGrid);

			lanceAttack(adversaire);
		}
   	}*/
}

void byebye(void){
	printf("\nVous êtes désormais déconnecté(e).\n");
	if(send(serveur, "-", 2, 0) < 0){
		perror("send()");
		exit(-1);
	}
	close(serveur);
	close(sock_client);
}

void ctrlC_Handler(int e){
    exit(0);
}

int main(int argc, char **argv){
  
    hostent * hostinfo; 		/* info sur une machine hote */
    char * prog; 				/* nom du programme */
    char * host; 				/* nom de la machine distante */
  
  	atexit(byebye);
    signal(SIGINT, ctrlC_Handler);
      
    if (argc != 2) {
		perror("usage : ./client.exe <adresse-serveur>");
		exit(1);
    }
   
    prog = argv[0];
    host = argv[1];
    
    printf("nom de l'executable : %s \n", prog);
    printf("adresse du serveur  : %s \n", host);
    
    if ((hostinfo = gethostbyname(host)) == NULL) {
		perror("erreur : impossible de trouver le serveur a partir de son adresse.");
		exit(1);
    }
    
    adresse_locale_globale.sin_addr = *(in_addr *) hostinfo->h_addr;
	adresse_locale_globale.sin_port = htons(5000);
	adresse_locale_globale.sin_family = AF_INET;

    printf("Numero de port pour la connexion au serveur : %d \n", ntohs(adresse_locale_globale.sin_port));
    
  	connexionAuServeur();
	
    exit(0);
}