#ifndef CMNFUNC_H
#define CMNFUNC_H

#include <stdlib.h> // malloc, free, rand, srand, RAND_MAX
#include <stdio.h>  // printf, fprintf
#include <errno.h>  // errno
#include <string.h> // strerror

void error(int err, char *msg); // Permet de stopper le programme avec un message d'erreur en cas de malfonction.
void simulate_processing(); // Permet de simuler un temps de processing.

#endif // CMNFUNC_H