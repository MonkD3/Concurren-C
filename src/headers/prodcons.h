#ifndef PRODCONS_H
#define PRODCONS_H

#include "cmnfunc.h"

#define BUFSIZE 8 // Taille du buffer 
#define NPROD 1024 // Nombre d'éléments produits (et consommés)

int algo;

int buffer[BUFSIZE]; // Buffer CIRCULAIRE
int idxput = 0; // Index auquel placer un élément produit
int idxtake = 0; // Index auquel prendre un élément à consommer

mutex_t mutex;
semaphore_t empty;
semaphore_t full;

void* producer(void* args);
void* consumer(void* args);

int produce();

#endif // PRODCONS_H