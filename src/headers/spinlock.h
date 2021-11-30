#ifndef SPINLOCK_H
#define SPINLOCK_H

#define TOTAL_CYCLES 6400 // Nombre total de cycles pour les test

mutex_t* sp; // spinlock utilisé pour tester les performances

int n_threads; // nombre de threads (donné par la CLI)
int n_sc; // Nombre de section critiques à exécuter par thread

void* target(void* args); // Target for the threads testing the TAS lock

#endif // SPINLOCK_H