CLIENT = client.exe
SERVEUR = serveur.exe

CC = gcc
CFLAGS = -std=gnu99 -pthread

all:
	$(CC) $(CFLAGS) grid.c serveur.c -o $(SERVEUR)
	$(CC) $(CFLAGS) grid.c client.c -o $(CLIENT)

clean:
	rm *.exe

