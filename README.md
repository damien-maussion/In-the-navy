# In-the-navy
Projet réseaux

Compiler avec gnu99


Pour le serveur : 
```
gcc -std=gnu99 -pthread grid.c serveur.c -o serveur.exe
./serveur.exe
```

Pour le client :
```
gcc -std=gnu99 -pthread client.c grid.c -o client.exe
./client.exe <adress-serveur>
```
