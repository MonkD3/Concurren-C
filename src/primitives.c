#include "./headers/primitives.h"

//=================================== Implémentation des mutex ===============================

spinlock_t* init_spinlock(){

    spinlock_t* spin = (spinlock_t*) malloc(sizeof(spinlock_t));
    if (spin == NULL) return NULL;
    spin->acquired = 0; // Non acquis

    return spin;
}


void destroy_spinlock(spinlock_t* spin){
    free(spin);
    spin = NULL;
}


int lock_tas(spinlock_t* spin){
    // Implémentation de l'algorithme test-and-set en asm 
    asm(
        "enter_tas:\n"
            "movl    $1, %%eax \n"    // %eax=1
            "xchgl   %%eax, %[lock] \n"    // echange les valeurs entre %eax et le spinlock
            "testl   %%eax, %%eax \n" // met le flag ZF à vrai si %eax contient 0
            "jnz     enter_tas"        // retour à enter: si ZF n'est pas vrai
        : [lock] "=m" (spin->acquired) // output
        : "m" (spin->acquired) // inputs
        : "%eax" // Registres utilisés 
        );

    return 0; // No problem
}


int tatas_helper(spinlock_t* spin){

    int out;

    asm(
        "movl    $1, %%eax\n"
        "xchgl   %%eax, %[lock]\n"
        "movl    %%eax, %[output]"
    : [lock] "=m" (spin->acquired), [output] "=m" (out)
    : "m" (spin->acquired)
    : "%eax"
    );

    return out;
}

int lock_tatas(spinlock_t* spin){

    while (tatas_helper(spin)){
        while(spin->acquired);
    }

    return 0;
}


int unlock_spinlock(spinlock_t* sp){
 
    asm(
        "movl    $0, %%eax\n" // %eax=0
        "xchgl   %%eax, %1"   // Remise à 0 du spinlock de manière atomique
        : "=m" (sp->acquired) // outputs
        : "m" (sp->acquired)  // inputs
        : "%eax" // Registres utilisés 
        );

    return 0; // No problem
}

//============================= Implémentation des sémaphores ===============================

spinsem_t* init_spinsem(int state){

    spinsem_t* sem = (spinsem_t*) malloc(sizeof(spinsem_t));
    if (sem == NULL) return NULL;

    sem->spinlock = init_spinlock();
    sem->state = state;

    return sem;
}


void destroy_spinsem(spinsem_t* spinsem){

    destroy_spinlock(spinsem->spinlock);
    free(spinsem);
    spinsem = NULL;
}


int wait_spinsem(spinsem_t* spinsem){

    while (1){
        lock_tatas(spinsem->spinlock);
        if (spinsem->state <= 0){
            unlock_spinlock(spinsem->spinlock);
            continue;
        }
        spinsem->state--;
        unlock_spinlock(spinsem->spinlock);
        break;
    }

    return 0;
}

int post_spinsem(spinsem_t* spinsem){
    lock_tatas(spinsem->spinlock);
    spinsem->state++;
    unlock_spinlock(spinsem->spinlock);

    return 0;
}