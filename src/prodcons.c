#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define BUFSIZE 8
#define NPROD 1024

int buffer[BUFSIZE];
int idxput;
int idxtake;

pthread_mutex_t mutex;
sem_t empty;
sem_t full;




void error(int err, char *msg) {
    fprintf(stderr,"%s a retourné %d message d'erreur : %s\n",msg,err,strerror(errno));
    exit(EXIT_FAILURE);
}

void simulate_processing() {
    // Simule la consommation de ressources CPU
    while(rand() > RAND_MAX/10000);
}


void* producer(void* args){
    int item;
    while(idxput < NPROD){
        // Simule un traitement (le calcul de l'élément à mettre dans le buffer)
        simulate_processing();

        sem_wait(&empty); // attente d'une place libre
        pthread_mutex_lock(&mutex);
        // section critique
        item = rand();
        buffer[(idxput++)%BUFSIZE] = item;

        pthread_mutex_unlock(&mutex);
        sem_post(&full); // il y a une place remplie en plus
    }
    return NULL;
}


void* consumer(void* args){
    int item;
    while(idxtake < NPROD){
        sem_wait(&full); // attente d'une place remplie
        pthread_mutex_lock(&mutex);
            // section critique
            item = buffer[(idxtake++)%BUFSIZE];
        pthread_mutex_unlock(&mutex);
        sem_post(&empty); // il y a une place libre en plus

        if (item); // This line avoids "unused-but-set-variable" from -Werror flag

        // Simule un traitement (le calcul effectué avec l'élément prit au buffer)
        simulate_processing();
    }
    return NULL;
}


int main(int argc, char* argv[]){

    int n_conso = atoi(argv[1]);
    int n_prod = atoi(argv[2]);

    if ((n_conso <= 0) || (n_prod <= 0)) {
        error(0, "Args");
    }

    pthread_t cons[n_prod];
    pthread_t prod[n_conso];
    int err;

    // Instancie les primitives de synchronisation
    pthread_mutex_init(&mutex, NULL);
    sem_init(&empty, 0 , BUFSIZE);
    sem_init(&full, 0 , 0); 

    // Initialisation des consumers :
    for (int i = 0; i < n_conso; i++){
        err = pthread_create(&cons[i], NULL, consumer, NULL);
        if (err != 0) error(err, "pthread_create");
    }

    // initialisation des prod :
    for (int i = 0; i < n_prod; i++){
        err = pthread_create(&prod[i], NULL, producer, NULL);
        if (err != 0) error(err, "pthread_create");
    }

    // Attente des cons :
    for (int i = 0; i < n_conso; i++) {
        err = pthread_join(cons[i], NULL);
        if(err!=0) error(err,"pthread_join");
    }

    // Attente des prod :
    for (int i = 0; i < n_prod; i++) {
        err = pthread_join(prod[i], NULL);
        if(err!=0) error(err,"pthread_join");
    }

    // Détruit les ressources utilisées :
    err = pthread_mutex_destroy(&mutex);
    err |= sem_destroy(&empty);
    err |= sem_destroy(&full); 
    if (err != 0) error(err, "Sem_destroy ou pthread_mutex_destroy");

    return EXIT_SUCCESS;
}