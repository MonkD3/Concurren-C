#include "./headers/cmnfunc.h" // error()
#include "./headers/primitives.h"
#include "./headers/philosophes.h" // philosophe(), CYCLES

algo_t algo;

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

    // Lecture du nombre de philosophes
    n_philo = atoi(argv[1]);
    if (n_philo <= 1) return EXIT_SUCCESS;

    // Type de primitives de synchronisation utilisée
    algo = POSIX; // Utilise les threads posix par défaut
    if (argc > 2){
        if (!strcmp(argv[2], "POSIX")) algo = POSIX;
        else if (!strcmp(argv[2], "TAS")) algo = TAS; 
        else if (!strcmp(argv[2], "TATAS")) algo = TATAS;
    }

    int i; 
    int err;
    int id[n_philo]; 
    pthread_t phil[n_philo];

    for (i = 0; i < n_philo; i++)
        id[i]=i;
    
    baguettes = (mutex_t*) malloc(sizeof(mutex_t) * n_philo);
    if (baguettes == NULL) error(0, "malloc");
    for (i = 0; i < n_philo; i++){
        err = init_mutex(&baguettes[i], algo);
        if (err) error(err, "init_mutex");
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
