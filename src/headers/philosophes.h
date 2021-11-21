#ifndef PHILOSOPHES_H
#define PHILOSOPHES_H

#define CYCLES 100000 // Les philosophes font 10000 cycles penser/manger

mutex_t* baguettes;

int n_philo; // Nombre de philosophes

void* philosophe ( void* arg );

#endif // PHILOSOPHES_H