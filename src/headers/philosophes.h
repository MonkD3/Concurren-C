#ifndef PHILOSOPHES_H
#define PHILOSOPHES_H

#define CYCLES 10000 // Les philosophes font 10000 cycles penser/manger

static pthread_mutex_t* baguette;
static int n_philo;

void* philosophe ( void* arg );

#endif // PHILOSOPHES_H