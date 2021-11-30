#include "./headers/cmnfunc.h" // stderr, errno, EXIT_FAILURE, rand, RAND_MAX, fprintf

void error(int err, char *msg) {
    fprintf(stderr,"%s a retourné %d message d'erreur : %s\n",msg,err,strerror(errno));
    exit(EXIT_FAILURE);
}

void __inline simulate_processing() {
    // Simule la consommation de ressources CPU
    while(rand() > RAND_MAX/10000);
}