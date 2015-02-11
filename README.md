# In-the-navy
Projet réseaux

Compiler avec gnu99


Pour le serveur : 
```
gcc -std=gnu99 -pthread grid.c serveur.c -o serveur.exe
./serveur.exe
```

Pour le clien :
```
gcc client.c -o client.exe
./client.exe <adress-serveur>
```