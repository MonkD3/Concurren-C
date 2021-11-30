#ifndef PRODCONS_H
#define PRODCONS_H

#include "cmnfunc.h"

#define BUFSIZE 8 // Taille du buffer 
#define NPROD 1024 // Nombre d'éléments produits (et consommés)

int buffer[BUFSIZE]; // Buffer CIRCULAIRE
int idxput = 0; // Index auquel placer un élément produit
int idxtake = 0; // Index auquel prendre un élément à consommer

mutex_t mutex; // Protège le buffer, idxput et idxtake 
semaphore_t empty; // Sert à déterminer si il y a des places libres
semaphore_t full; // Sert à déterminer si il y a des items dans le buffer

void* producer(void* args);
void* consumer(void* args);

int produce();

#endif // PRODCONS_H