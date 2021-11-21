
#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include <stdlib.h>

int algo; // 0 pour posix, 1 pour TAS et 2 pour TATAS

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

#endif // PRIMITIVES_H