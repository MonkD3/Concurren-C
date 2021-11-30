
#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include <pthread.h>   // pthread_t, pthread_create, pthread_join
#include <semaphore.h> // sem_t, sem_wait, sem_post
#include <limits.h>    // INT_MAX, INT_MIN

int algo; // 0 pour POSIX, 1 pour TAS et 2 pour TATAS

//============================ Implémentation du Spinlock ==============================

typedef struct {
    int acquired;
} spinlock_t;

spinlock_t* init_spinlock(); // Initialise un nouveau spinlock
void destroy_spinlock(spinlock_t*); // Libère la mémoire allouée pour un spinlock

int lock_tas(spinlock_t*); // Implémente l'algorithme test-and-set
int lock_tatas(spinlock_t*); // Implémente l'algorithme test-and-test-and-set
int tatas_helper(spinlock_t*); // Permet de faciliter l'implémentation du spinlock TATAS (avec comme coût un léger overhead)
int unlock_spinlock(spinlock_t*); // Libère le spinlock de manière atomique

//============================ Implémentation de la Sémaphore =========================

typedef struct {
    int state; // Nombre de "places" restantes
    spinlock_t* spinlock; // Spinlock associé à la sémaphore
} spinsem_t;

spinsem_t* init_spinsem(int state); // Initialise une nouvelle semaphore à l'aide d'un spinlock
void destroy_spinsem(spinsem_t*); // Libère la mémoire allouée pour une sémaphore

int wait_spinsem(spinsem_t*); // Attend sur la sémaphore 
int post_spinsem(spinsem_t*); // Poste la sémaphore


// =========================== Interface de mutex ================================
// Permet d'avoir seulement 2 fonctions génériques lock() et unlock() gérant
// les mutex POSIX ainsi que les mutex implémenté dans primitives.h
typedef struct {
    int type; // Type du mutex : 0 pour POSIX, 1 pour TAS, 2 pour TATAS
    union {
        pthread_mutex_t* posix;
        spinlock_t* spinlock;
    }; // Type de mutex suivant la valeur fournie pour type
} mutex_t;

int init_mutex(mutex_t*, int algo); // Initialise un nouveau mutex de type "algo"
int destroy_mutex(mutex_t*); // Libère la mémoire allouée à un mutex

int lock(mutex_t*); // Lock un mutex
int unlock(mutex_t*); // Unlock un mutex

// ============================ Interface de sémaphores ===========================
// Permet d'avoir seulement 2 fonctions génériques wait() et post() gérant les 
// sémaphores POSIX ainsi que les sémaphores implémentées dans primmitives.h
typedef struct {
    int type; // Type de la sémaphore : 0 pour POSIX, 1 pour TAS, 2 pour TATAS
    union{
        sem_t* posix;
        spinsem_t* spinsem;
    }; // Type de sémaphore suivant la valeur fournier pour type
} semaphore_t;

int init_semaphore(semaphore_t*, int algo, int val); // Initialise une sémaphore de type "algo" avec "val" places
int destroy_semaphore(semaphore_t*); // Désalloue la mémoire allouée pour cette sémaphore

int wait(semaphore_t*); // Attend sur la sémaphore (décrémente son compteur)
int post(semaphore_t*); // Poste une sémaphore (incrémente son compteur)

#endif // PRIMITIVES_H