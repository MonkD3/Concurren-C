#include "./headers/cmnfunc.h" // stderr, errno, EXIT_FAILURE, rand, RAND_MAX, fprintf
#include "./headers/primitives.h"

void error(int err, char *msg) {
    fprintf(stderr,"%s a retourné %d message d'erreur : %s\n",msg,err,strerror(errno));
    exit(EXIT_FAILURE);
}

void simulate_processing() {
    // Simule la consommation de ressources CPU
    while(rand() > RAND_MAX/10000);
}


// ======================= Interfaces mutex ======================================

int init_mutex(mutex_t* mut, int algo){
    int err = 0;

    mut->type = algo;
    if (algo == 0){
        mut->posix = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t));
        err = pthread_mutex_init(mut->posix, NULL);
        if (err != 0) error(err, "pthread_mutex_init");
    }
    else {
        mut->spinlock = init_spinlock();
        if (mut->spinlock == NULL) return -1;
    }

    return err;
}

int destroy_mutex(mutex_t* mutex){

    int err = 0;
    if (mutex->type == 0){
        err = pthread_mutex_destroy(mutex->posix);
        free(mutex->posix);
    } 
    else destroy_spinlock(mutex->spinlock);

    return err;
}


int lock(mutex_t* mutex){

    int err = 0;

    switch (mutex->type){
    case 0:
        err = pthread_mutex_lock(mutex->posix);
        break;
    case 1:
        err = lock_tas(mutex->spinlock);
        break;
    case 2:
        err = lock_tatas(mutex->spinlock);
        break;

    default:
        break;
    }

    return err;
}

int unlock(mutex_t* mutex){

    int err = 0;

    switch (mutex->type){
    case 0:
        err = pthread_mutex_unlock(mutex->posix);
        break;
    case 1: // Unlock un spinlock TAS ou TATAS utilise la même fonction
    case 2:
        err = unlock_spinlock(mutex->spinlock);
        break;
    default:
        break;
    }

    return err;
}


// ======================= Interface sémaphores ==================================

int init_semaphore(semaphore_t* sem, int algo, int val){
    int err = 0;

    sem->type = algo;
    if (algo == 0){
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
    if (sem->type == 0){
        err = sem_destroy(sem->posix);
        free(sem->posix);
    } 
    else destroy_spinsem(sem->spinsem);

    return err;
}


int wait(semaphore_t* sem){

    int err = 0;

    switch (sem->type){
    case 0:
        err = sem_wait(sem->posix);
        break;
    case 1: // Les sémaphores sont implémentées en utilisant l'algorithme TATAS
    case 2:
        err = wait_spinsem(sem->spinsem);
        break;

    default:
        break;
    }

    return err;
}

int post(semaphore_t* sem){

    int err = 0;

    switch (sem->type){
    case 0:
        err = sem_post(sem->posix);
        break;
    case 1: // Unlock un sémaphore TAS ou TATAS utilise la même fonction
    case 2:
        err = post_spinsem(sem->spinsem);
        break;
    default:
        break;
    }

    return err;
}