CLIENT = client.exe
SERVEUR = serveur.exe

CC = gcc
CFLAGS = -std=gnu99 -pthread

all:
	$(CC) $(CFLAGS) grid.c serveurv1.c -o $(SERVEUR)
	$(CC) $(CFLAGS) grid.c clientv1.c -o $(CLIENT)

clean:
	rm *.exe

