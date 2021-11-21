#include "./headers/cmnfunc.h"
#include "./headers/spinlock.h"


void* target(void* args){
    
    const int algo = *((int*) args); // Quel algo utiliser : 0 pour TAS, 1 pour TATAS
    for (int i = 0; i < n_sc; i++){
        // Try to lock the spinlock using the right algorithm
        if (algo) lock_tatas(sp);
        else lock_tas(sp);

        // simulate processing
        simulate_processing();
        // unlock the spinlock
        unlock_spinlock(sp);
    }

    return NULL;
}


int main(int argc, char* argv[]){

    // Get the number of threads and the number of SC/threads
    if ((n_threads = atoi(argv[1])) <= 0) return EXIT_FAILURE;
    n_sc = TOTAL_CYCLES / n_threads;

    // Choisis l'algorithme de lock à utiliser
    int algo;
    if (!strcasecmp(argv[2], "TAS")) algo = 0;
    else if (!strcasecmp(argv[2], "TATAS")) algo = 1;
    else error(0, "Wrong arguments");

    int err;


    pthread_t threads[n_threads];
    if ((sp = init_spinlock()) == NULL) return EXIT_FAILURE; // malloc the spinlock

    
    for (int i = 0; i < n_threads; i++){
        if ((err = pthread_create(&threads[i], NULL, target, (void*)&algo)) != 0) error(err, "pthread_create");
    }

    for (int i = 0; i < n_threads; i++){
        if ((err = pthread_join(threads[i], NULL)) != 0) error(err, "pthread_join");
    }


    destroy_spinlock(sp); // destroy the spinlock
    return EXIT_SUCCESS;
}