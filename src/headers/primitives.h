
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

/** 
 * Essaie d'acquérir le spinlock UNE SEULE fois (atomiquement) et retourne la valeur  
 * qui était comprise dans le spinlock (0 si il était libre, 1 si il était lock)
 */
int tatas_helper(spinlock_t*); 
int lock_tas(spinlock_t*); // Implémente l'algorithme test-and-set
int lock_tatas(spinlock_t*); // Implémente l'algorithme test-and-test-and-set
int unlock_spinlock(spinlock_t*); // Libère le spinlock de manière atomique

//============================ Implémentation de la Sémaphore =========================

typedef struct {
    int state;
    spinlock_t* spinlock;
} spinsem_t;

spinsem_t* init_spinsem(int state); // Initialise une nouvelle semaphore à l'aide d'un spinlock
void destroy_spinsem(spinsem_t*); // Libère la mémoire allouée pour une sémaphore

int wait_spinsem(spinsem_t*); // Attend sur la sémaphore 
int post_spinsem(spinsem_t*); // Poste la sémaphore


// =========================== Interface de mutex ================================
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

// ============================ Interface de sémaphores ===========================
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

#endif // PRIMITIVES_H