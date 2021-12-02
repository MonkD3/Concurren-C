#include "./headers/primitives.h"
#include "./headers/cmnfunc.h"

//=================================== Implémentation des mutex ===============================

spinlock_t* init_spinlock(){

    spinlock_t* spin = (spinlock_t*) malloc(sizeof(spinlock_t));
    if (spin == NULL) return NULL; 
    spin->acquired = 0; // Non acquis

    return spin;
}


void destroy_spinlock(spinlock_t* spin){
    free(spin);
    spin = NULL;
}


int lock_tas(spinlock_t* spin){
    // Implémentation de l'algorithme test-and-set en asm 
    asm(
        "enter_tas:\n"
            "movl    $1, %%eax \n"    // %eax=1
            "xchgl   %%eax, %[lock] \n"    // echange les valeurs entre %eax et le spinlock
            "testl   %%eax, %%eax \n" // met le flag ZF à vrai si %eax contient 0
            "jnz     enter_tas"        // retour à enter: si ZF n'est pas vrai
        : [lock] "=m" (spin->acquired) // output
        : "m" (spin->acquired) // inputs
        : "%eax" // Registres utilisés 
        );

    return 0; // No problem
}


int tatas_helper(spinlock_t* spin){

    int out;

    asm(
        "movl    $1, %%eax\n"
        "xchgl   %%eax, %[lock]\n"
        "movl    %%eax, %[output]"
    : [lock] "=m" (spin->acquired), [output] "=m" (out)
    : "m" (spin->acquired)
    : "%eax"
    );

    return out;
}

int lock_tatas(spinlock_t* spin){

    while (tatas_helper(spin)){
        while(spin->acquired);
    }

    return 0;
}


int unlock_spinlock(spinlock_t* sp){
 
    asm(
        "movl    $0, %%eax\n" // %eax=0
        "xchgl   %%eax, %1"   // Remise à 0 du spinlock de manière atomique
        : "=m" (sp->acquired) // outputs
        : "m" (sp->acquired)  // inputs
        : "%eax" // Registres utilisés 
        );

    return 0; // No problem
}

//============================= Implémentation des sémaphores ===============================

spinsem_t* init_spinsem(int state){

    spinsem_t* sem = (spinsem_t*) malloc(sizeof(spinsem_t));
    if (sem == NULL) return NULL;

    sem->spinlock = init_spinlock();
    sem->state = state;

    return sem;
}


void destroy_spinsem(spinsem_t* spinsem){

    destroy_spinlock(spinsem->spinlock);
    free(spinsem);
    spinsem = NULL;
}


int wait_spinsem(spinsem_t* spinsem){

    while (1){
        lock_tatas(spinsem->spinlock);
        if (spinsem->state <= 0){
            unlock_spinlock(spinsem->spinlock);
            continue;
        }
        spinsem->state--;
        unlock_spinlock(spinsem->spinlock);
        break;
    }

    return 0;
}

int post_spinsem(spinsem_t* spinsem){
    lock_tatas(spinsem->spinlock);
    spinsem->state++;
    unlock_spinlock(spinsem->spinlock);

    return 0;
}


// ============================ Interface mutex ==========================

int init_mutex(mutex_t* mut, algo_t algo){
    int err = 0;

    mut->type = algo;
    if (algo == POSIX){
        mut->posix = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t));
        err = pthread_mutex_init(mut->posix, NULL);
        if (err != 0) error(err, "pthread_mutex_init");
    }
    else {
        mut->spinlock = init_spinlock();
        if (mut->spinlock == NULL) error(0, "init_spinlock"); 
    }

    return err;
}

int destroy_mutex(mutex_t* mutex){

    int err = 0;
    if (mutex->type == POSIX){
        err = pthread_mutex_destroy(mutex->posix);
        free(mutex->posix);
    } 
    else destroy_spinlock(mutex->spinlock);

    return err;
}


int lock(mutex_t* mutex){

    int err = 0;

    switch (mutex->type){
    case POSIX:
        err = pthread_mutex_lock(mutex->posix);
        break;
    case TAS:
        err = lock_tas(mutex->spinlock);
        break;
    case TATAS:
        err = lock_tatas(mutex->spinlock);
        break;
    }

    return err;
}

int unlock(mutex_t* mutex){

    int err = 0;

    switch (mutex->type){
    case POSIX:
        err = pthread_mutex_unlock(mutex->posix);
        break;
    case TAS: // Unlock un spinlock TAS ou TATAS utilise la même fonction
    case TATAS:
        err = unlock_spinlock(mutex->spinlock);
        break;
    }

    return err;
}


// ======================= Interface sémaphores ==================================

int init_semaphore(semaphore_t* sem, algo_t algo, int val){
    int err = 0;

    sem->type = algo;
    if (algo == POSIX){
        sem->posix = (sem_t*) malloc(sizeof(sem_t));
        if (sem->posix == NULL) error(0, "malloc");
        err = sem_init(sem->posix, 0, val);
        if (err != 0) error(err, "pthread_mutex_init");
    }
    else {
        sem->spinsem = init_spinsem(val);
        if (sem->spinsem == NULL) error(0, "init_spinsem");
    }

    return err;
}


int destroy_semaphore(semaphore_t* sem){

    int err = 0;
    if (sem->type == POSIX){
        err = sem_destroy(sem->posix);
        free(sem->posix);
    } 
    else destroy_spinsem(sem->spinsem);

    return err;
}


int wait(semaphore_t* sem){

    int err = 0;

    switch (sem->type){
    case POSIX:
        err = sem_wait(sem->posix);
        break;
    case TAS: // Les sémaphores sont implémentées en utilisant l'algorithme TATAS
    case TATAS:
        err = wait_spinsem(sem->spinsem);
        break;
    }

    return err;
}

int post(semaphore_t* sem){

    int err = 0;

    switch (sem->type){
    case POSIX:
        err = sem_post(sem->posix);
        break;
    case TAS: // Unlock un sémaphore TAS ou TATAS utilise la même fonction
    case TATAS:
        err = post_spinsem(sem->spinsem);
        break;
    }

    return err;
}