#include "./headers/cmnfunc.h" // error(), simulate_processing()
#include "./headers/readwrt.h" // reader(), writer()


void * writer(void* args){
    while (1) {

        // Le writer est censé "prévoir" les données à écrire ici.

        pthread_mutex_lock(&mutex_writecount);
            // section critique - writecount
            if (write_total >= WR_CYCLES){
                pthread_mutex_unlock(&mutex_writecount);
                break;
            }
            write_total++;

            writecount++;
            if(writecount==1) {
            // premier writer arrive
            sem_wait(&rsem);
            }
        pthread_mutex_unlock(&mutex_writecount);

        sem_wait(&wsem);
            // section critique, un seul writer à la fois
            simulate_processing(); 
        sem_post(&wsem);

        pthread_mutex_lock(&mutex_writecount);
            // section critique - writecount
            writecount--;
            if(writecount==0) {
            // départ du dernier writer
            sem_post(&rsem);
            }
        pthread_mutex_unlock(&mutex_writecount);
    }

    return NULL;
}


void * reader(void* args){
    while(1) {
        pthread_mutex_lock(&z);
            // exclusion mutuelle, un seul reader en attente sur rsem
            sem_wait(&rsem);
                pthread_mutex_lock(&mutex_readcount);
                    // exclusion mutuelle, readercount

                    if (read_total >= RD_CYCLES){
                        pthread_mutex_unlock(&mutex_readcount);
                        sem_post(&rsem);
                        pthread_mutex_unlock(&z);
                        break;
                    }
                    read_total++;

                    readcount++;
                    if (readcount==1) {
                        // arrivée du premier reader
                        sem_wait(&wsem);
                    }
                pthread_mutex_unlock(&mutex_readcount);
            sem_post(&rsem);  // libération du prochain reader
        pthread_mutex_unlock(&z);

        simulate_processing();

        pthread_mutex_lock(&mutex_readcount);
            // exclusion mutuelle, readcount
            readcount--;
            if(readcount==0) {
                // départ du dernier reader
                sem_post(&wsem);
            }
        pthread_mutex_unlock(&mutex_readcount);

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

    pthread_t readers[n_reader];
    pthread_t writers[n_writer];
    int err;

    // Instancie les sémaphores
    sem_init(&wsem, 0, 1);
    sem_init(&rsem, 0, 1);
    pthread_mutex_init(&mutex_readcount, NULL);
    pthread_mutex_init(&mutex_writecount, NULL);
    pthread_mutex_init(&z, NULL);

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
    err = sem_destroy(&wsem);
    err |= sem_destroy(&rsem);
    err |= pthread_mutex_destroy(&mutex_writecount);
    err |= pthread_mutex_destroy(&mutex_readcount);
    err |= pthread_mutex_destroy(&z);
    if (err != 0) error(err, "Sem_destroy ou pthread_mutex_destroy");

    return EXIT_SUCCESS;
}