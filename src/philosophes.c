#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#define CYCLES 10000

static pthread_mutex_t* baguette;
static int n_philo;

void error(int err, char *msg) {
    fprintf(stderr,"%s a retournÃ© %d message d'erreur : %s\n",msg,err,strerror(errno));
    exit(EXIT_FAILURE);
}



void* philosophe ( void* arg ){

    int *id=(int *) arg;
    int left = *id;
    int right = (left + 1) % n_philo;

    for (int i = 0; i < CYCLES; i++) {
        // philosophe pense
        if(left<right) {
        pthread_mutex_lock(&baguette[left]);
        pthread_mutex_lock(&baguette[right]);
        }
        else {
        pthread_mutex_lock(&baguette[right]);
        pthread_mutex_lock(&baguette[left]);
        }

        // Le philosophe mange ici

        pthread_mutex_unlock(&baguette[left]);
        pthread_mutex_unlock(&baguette[right]);
    }
    return (NULL);
}




int main ( int argc, char *argv[]){

    n_philo = atoi(argv[1]);
    if (n_philo == 0){
        error(0, "n_philo");
    }

    int i; // To iterate
    int id[n_philo]; 
    int err;
    pthread_t phil[n_philo];

    for (i = 0; i < n_philo; i++)
        id[i]=i;

    
    // Initialisation des baguettes
    if((baguette = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t) * n_philo)) < 0) error(-1, "malloc");
    for (i = 0; i < n_philo; i++) {
        err=pthread_mutex_init( &baguette[i], NULL);
        if(err!=0) error(err,"pthread_mutex_init");
    }

    // Initialisation des philosophes
    for (i = 0; i < n_philo; i++) {
        err=pthread_create(&phil[i], NULL, philosophe, (void*)&(id[i]) );
        if(err!=0) error(err,"pthread_create");
    }

    // Attente des philosophes
    for (i = 0; i < n_philo; i++) {
        err = pthread_join(phil[i], NULL);
        if(err!=0) error(err,"pthread_join");
    }

    // Libération des ressources des baguettes
    for (i = 0; i < n_philo; i++) {
        err = pthread_mutex_destroy(&baguette[i]);
        if(err!=0) error(err,"pthread_mutex_destroy");
    }
    free(baguette);
    baguette = NULL;

    return (EXIT_SUCCESS);
}