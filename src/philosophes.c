#include "./headers/cmnfunc.h" // error()
#include "./headers/primitives.h"
#include "./headers/philosophes.h" // philosophe(), CYCLES


void eat(){
    // Le philosophe mange
}

void think(){
    // Le philosophe pense
}

void* philosophe ( void* arg ){

    int *id=(int *) arg;
    int left = *id;
    int right = (left + 1) % n_philo;
    int i;

    for (i = 0; i < CYCLES; i++) {

        think();

        if(left<right) {
            lock(&baguettes[left]);
            lock(&baguettes[right]);
        }
        else {
            lock(&baguettes[right]);
            lock(&baguettes[left]);
        }

        eat();

        unlock(&baguettes[left]);
        unlock(&baguettes[right]);
    }

    return (NULL);
}


int main ( int argc, char *argv[]){

    n_philo = atoi(argv[1]);
    if (n_philo <= 1){
        return EXIT_SUCCESS;
    }

    if (!strcmp(argv[2], "POSIX")) algo = 0;
    else if (!strcmp(argv[2], "TAS")) algo = 1; 
    else if (!strcmp(argv[2], "TATAS")) algo = 2;
    else algo = 0; // Utilise les threads posix par défaut


    int i; // To iterate
    int id[n_philo]; 
    int err;
    pthread_t phil[n_philo];

    for (i = 0; i < n_philo; i++)
        id[i]=i;
    
    baguettes = (mutex_t*) malloc(sizeof(mutex_t) * n_philo);
    for (i = 0; i < n_philo; i++){
        err = init_mutex(&baguettes[i], algo);
        if (&baguettes[i] == NULL) error(0, "init_mutex");
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

    for (i = 0; i < n_philo; i++){
        err = destroy_mutex(&baguettes[i]);
        if (err != 0) error(err, "destroy_mutex");
    }
    free(baguettes);

    return EXIT_SUCCESS;
}