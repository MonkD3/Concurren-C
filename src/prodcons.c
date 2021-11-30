#include "./headers/primitives.h"
#include "./headers/cmnfunc.h" // error(), simulate_processing()
#include "./headers/prodcons.h" // producer(), consumer()


int __inline produce(){
    // - rand() retourne un élément dans la plage [0;2^31 - 1]
    // - INT_MIN vaut -2^31
    // - (rand() >= INT_MAX/2) vaut 0 avec une probabilité de 0.5 et 1 avec une probabilité de 0.5
    // Informellement nous avons :  
    //       -2^31 + [0;2*(2^31 - 1)] + 1 = [-2^31 + 1; 2^31 -1]
    // ou    -2^31 + [0;2*(2^31 - 1)] = [-2^31 ; 2^31 - 2]
    // Ce qui nous donne bien : [-2^31; 2^31 - 1]
    return INT_MIN + rand() + rand() + (rand() >= INT_MAX/2);   
}

void* producer(void* args){
    int item;
    while(1){
        // Simule un traitement (le calcul de l'élément à mettre dans le buffer)
        simulate_processing();
        item = produce();
        wait(&empty); // attente d'une place libre
            lock(&mutex);
                // section critique
                if (idxput >= NPROD){
                    unlock(&mutex);
                    post(&full);
                    post(&empty);
                    break;
                }
                buffer[(idxput++)%BUFSIZE] = item;
            unlock(&mutex);
        post(&full); // il y a une place remplie en plus
    }
    return NULL;
}


void* consumer(void* args){
    int item;
    while(1){
        wait(&full); // attente d'une place remplie
            lock(&mutex);
                if (idxtake >= NPROD){
                    unlock(&mutex);
                    post(&empty);
                    post(&full);
                    break;
                }
                // section critique
                item = buffer[(idxtake++)%BUFSIZE];
            unlock(&mutex);
        post(&empty); // il y a une place libre en plus

        if (item); // This line avoids "unused-but-set-variable" from -Werror flag

        // Simule un traitement (le calcul effectué avec l'élément prit au buffer)
        simulate_processing();
    }
    return NULL;
}


int main(int argc, char* argv[]){
    srand(time(NULL));

    int n_conso = atoi(argv[1]);
    int n_prod = atoi(argv[2]);

    if ((n_conso <= 0) || (n_prod <= 0)) {
        return EXIT_SUCCESS;
    }

    char* arg = argv[3];
    if (!strcmp(arg, "POSIX")) algo = 0;
    else if (!strcmp(arg, "TAS")) algo = 1; 
    else if (!strcmp(arg, "TATAS")) algo = 2;
    else algo = 0; // Utilise les threads posix par défaut

    pthread_t cons[n_conso];
    pthread_t prod[n_prod];
    int err;

    // Instancie les primitives de synchronisation
    if (init_mutex(&mutex, algo) < 0) error(0, "init_mutex");
    if (init_semaphore(&empty, algo, BUFSIZE) < 0) error(0, "init_semaphore");
    if (init_semaphore(&full, algo, 0) < 0) error(0, "init_semaphore");

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
    
    err = destroy_mutex(&mutex);
    err |= destroy_semaphore(&empty);
    err |= destroy_semaphore(&full); 
    if (err != 0) error(err, "Sem_destroy ou pthread_mutex_destroy");


    return EXIT_SUCCESS;
}