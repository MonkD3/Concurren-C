#include "./headers/cmnfunc.h"
#include "./headers/primitives.h"
#include "./headers/spinlock.h"


void* target(void* args){
    
    for (int i = 0; i < n_sc; i++){
        // Try to lock the spinlock using the right algorithm
        lock(sp);
        // simulate processing
        simulate_processing();
        // unlock the spinlock
        unlock(sp);
    }

    return NULL;
}


int main(int argc, char* argv[]){

    // Get the number of threads and the number of SC/threads
    if ((n_threads = atoi(argv[1])) <= 0) return EXIT_FAILURE;
    n_sc = TOTAL_CYCLES / n_threads;

    // Choisis l'algorithme de lock à utiliser
    if (!strcmp(argv[2], "POSIX")) algo = 0;
    else if (!strcmp(argv[2], "TAS")) algo = 1;
    else if (!strcmp(argv[2], "TATAS")) algo = 2;
    else error(0, "Wrong arguments");

    int err;

    pthread_t threads[n_threads];

    sp = (mutex_t*) malloc(sizeof(mutex_t));
    if (sp == NULL) return EXIT_FAILURE;
    if (init_mutex(sp, algo) != 0) return EXIT_FAILURE; // malloc the spinlock
    
    for (int i = 0; i < n_threads; i++){
        if ((err = pthread_create(&threads[i], NULL, target, (void*)&algo)) != 0) error(err, "pthread_create");
    }

    for (int i = 0; i < n_threads; i++){
        if ((err = pthread_join(threads[i], NULL)) != 0) error(err, "pthread_join");
    }

    destroy_mutex(sp); // destroy the spinlock
    free(sp);
    return EXIT_SUCCESS;
}