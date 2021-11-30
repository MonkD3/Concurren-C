#ifndef PHILOSOPHES_H
#define PHILOSOPHES_H

#define CYCLES 100000 // Les philosophes font 10000 cycles penser/manger

mutex_t* baguettes; // Baguettes des philosophes.

int n_philo; // Nombre de philosophes

void* philosophe ( void* arg ); // Fait effectuer CYCLES cycles manger/penser à un philosophe.

#endif // PHILOSOPHES_H