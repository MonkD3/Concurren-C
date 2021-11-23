#ifndef CMNFUNC_H
#define CMNFUNC_H

#include <stdlib.h> // malloc, free, rand, srand, RAND_MAX
#include <stdio.h>  // printf, fprintf
#include <errno.h>  // errno
#include <string.h> // strerror

void error(int err, char *msg);
void simulate_processing();

#endif // CMNFUNC_H