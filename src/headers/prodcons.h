#ifndef PRODCONS_H
#define PRODCONS_H

#include <semaphore.h>
#include <limits.h>

#define BUFSIZE 8 // Taille du buffer 
#define NPROD 1024 // Nombre d'éléments produits (et consommés)


int buffer[BUFSIZE]; // Buffer CIRCULAIRE
int idxput; // Index auquel placer un élément produit
int idxtake; // Index auquel prendre un élément à consommer

pthread_mutex_t mutex;
sem_t empty;
sem_t full;

void* producer(void* args);
void* consumer(void* args);

int produce();

#endif // PRODCONS_H