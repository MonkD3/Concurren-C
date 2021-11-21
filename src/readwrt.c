#include "./headers/cmnfunc.h" // error(), simulate_processing()
#include "./headers/primitives.h"
#include "./headers/readwrt.h" // reader(), writer()


void * writer(void* args){
    while (1) {
        // Le writer est censé "prévoir" les données à écrire ici.

        lock(&mutex_writecount);
            // section critique - writecount
            if (write_total >= WR_CYCLES){
                unlock(&mutex_writecount);
                break;
            }
            write_total++;

            writecount++;
            if(writecount==1) {
                // premier writer arrive
                wait(&rsem);
            }
        unlock(&mutex_writecount);

        wait(&wsem);
            // section critique, un seul writer à la fois
            simulate_processing(); 
        post(&wsem);

        lock(&mutex_writecount);
            // section critique - writecount
            writecount--;
            if(writecount==0) {
                // départ du dernier writer
                post(&rsem);
            }
        unlock(&mutex_writecount);
    }

    return NULL;
}


void * reader(void* args){
    while(1) {
        lock(&z);
            // exclusion mutuelle, un seul reader en attente sur rsem
            wait(&rsem);
                lock(&mutex_readcount);
                    // exclusion mutuelle, readercount

                    if (read_total >= RD_CYCLES){
                        unlock(&mutex_readcount);
                        post(&rsem);
                        unlock(&z);
                        break;
                    }
                    read_total++;

                    readcount++;
                    if (readcount==1) {
                        // arrivée du premier reader
                        wait(&wsem);
                    }
                unlock(&mutex_readcount);
            post(&rsem);  // libération du prochain reader
        unlock(&z);

        simulate_processing();

        lock(&mutex_readcount);
            // exclusion mutuelle, readcount
            readcount--;
            if(readcount==0) {
                // départ du dernier reader
                post(&wsem);
            }
        unlock(&mutex_readcount);

        // Le reader est censé process l'information lue ici.
    }

    return NULL;
}



int main(int argc, char* argv[]){

    int n_reader = atoi(argv[1]);
    int n_writer = atoi(argv[2]);

    if ((n_writer <= 0) || (n_reader <= 0)) {
        return EXIT_SUCCESS;
    }

    char* arg = argv[3];
    if (!strcasecmp(arg, "POSIX")) algo = 0;
    else if (!strcasecmp(arg, "TAS")) algo = 1; 
    else if (!strcasecmp(arg, "TATAS")) algo = 2;
    else algo = 0; // Utilise les threads posix par défaut

    pthread_t readers[n_reader];
    pthread_t writers[n_writer];
    int err;

    // Instancie les sémaphores
    if ((err = init_semaphore(&wsem, algo, 1)) < 0) error(err, "init_semaphore");
    if ((err = init_semaphore(&rsem, algo, 1)) < 0) error(err, "init_semaphore");
    if ((err = init_mutex(&mutex_readcount, algo)) < 0) error(err, "init_mutex");
    if ((err = init_mutex(&mutex_writecount, algo)) < 0) error(err, "init_mutex");
    if ((err = init_mutex(&z, algo)) < 0) error(err, "init_mutex");

    // Initialisation des readers :
    for (int i = 0; i < n_reader; i++){
        err = pthread_create(&readers[i], NULL, reader, NULL);
        if (err != 0) error(err, "pthread_create");
    }

    // initialisation des writers :
    for (int i = 0; i < n_writer; i++){
        err = pthread_create(&writers[i], NULL, writer, NULL);
        if (err != 0) error(err, "pthread_create");
    }

    // Attente des readers :
    for (int i = 0; i < n_reader; i++) {
        err = pthread_join(readers[i], NULL);
        if(err!=0) error(err,"pthread_join");
    }

    // Attente des writers :
    for (int i = 0; i < n_writer; i++) {
        err = pthread_join(writers[i], NULL);
        if(err!=0) error(err,"pthread_join");
    }

    // Détruit les ressources utilisées :
    err = destroy_semaphore(&wsem);
    err |= destroy_semaphore(&rsem);
    err |= destroy_mutex(&mutex_writecount);
    err |= destroy_mutex(&mutex_readcount);
    err |= destroy_mutex(&z);
    if (err != 0) error(err, "Sem_destroy ou pthread_mutex_destroy");

    return EXIT_SUCCESS;
}