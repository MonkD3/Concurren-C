#include "./headers/prodcons.h" // producer(), consumer()
#include "./headers/cmnfunc.h" // error(), simulate_processing()


int produce(){
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

        sem_wait(&empty); // attente d'une place libre
            pthread_mutex_lock(&mutex);
                // section critique
                if (idxput >= NPROD){
                    pthread_mutex_unlock(&mutex);
                    sem_post(&full);
                    sem_post(&empty);
                    break;
                }
                buffer[(idxput++)%BUFSIZE] = item;
            pthread_mutex_unlock(&mutex);
        sem_post(&full); // il y a une place remplie en plus
    }
    return NULL;
}


void* consumer(void* args){
    int item;
    while(1){
        sem_wait(&full); // attente d'une place remplie
            pthread_mutex_lock(&mutex);
                if (idxtake >= NPROD){
                    pthread_mutex_unlock(&mutex);
                    sem_post(&empty);
                    sem_post(&full);
                    break;
                }
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
    srand(time(NULL));
    int n_conso = atoi(argv[1]);
    int n_prod = atoi(argv[2]);

    if ((n_conso <= 0) || (n_prod <= 0)) {
        return EXIT_SUCCESS;
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