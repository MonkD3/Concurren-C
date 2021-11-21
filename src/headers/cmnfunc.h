#ifndef CMNFUNC_H
#define CMNFUNC_H

#include <pthread.h>
#include <semaphore.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "primitives.h"

void error(int err, char *msg);
void simulate_processing();


// =========================== Interfaces de mutex ================================
// Permet d'avoir seulement 2 fonctions génériques lock() et unlock() gérant
// les mutex POSIX ainsi que les mutex implémenté dans primitives.h
typedef struct {
    int type;
    union {
        pthread_mutex_t* posix;
        spinlock_t* spinlock;
    };
} mutex_t;

int init_mutex(mutex_t*, int algo);
int destroy_mutex(mutex_t*);

int lock(mutex_t*);
int unlock(mutex_t*);

// ============================ Interfaces de sémaphores ===========================
// Permet d'avoir seulement 2 fonctions génériques wait() et post() gérant les 
// sémaphores POSIX ainsi que les sémaphores implémentées dans primmitives.h
typedef struct {
    int type;
    union{
        sem_t* posix;
        spinsem_t* spinsem;
    };
} semaphore_t;

int init_semaphore(semaphore_t*, int algo, int val);
int destroy_semaphore(semaphore_t*);

int wait(semaphore_t*);
int post(semaphore_t*);

#endif // CMNFUNC_H