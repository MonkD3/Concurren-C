#ifndef CMNFUNC_H
#define CMNFUNC_H

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

void error(int err, char *msg);

void simulate_processing();

#endif // CMNFUNC_H