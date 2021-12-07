# concurren-C

## Introduction 
Ce projet contient de nombreux fichiers permettant d'analyser les performances de plusieurs algorithmes classiques de synchronisations :
* Le problème des philosophes
* Le problème des producteurs-consommateurs
* Le problème des lecteurs-écrivains

Nous allons tester ces trois algorithmes en fonction de deux paramètres 
- Le nombre de thread 
- Les primitives de synchronisation utilisées

Le nombre de thread est un entier compris entre 1 et 2n où n est le nombre de coeurs du processeur utilisé. Les primitives de synchronisations seront soit les mutex et sémaphores POSIX définies dans ``<pthread.h>`` et ``<semaphore.h>`` soit les verrous à attente active et sémaphores construit sur base de ces verrous définies dans ``"primitives.h"``.

## Compilation

La compilation de ce projet a été faite avec ces outils :
* GCC version 9.3.0
* Ubuntu 20.04.2 LTS
* GNU Make version 4.2.1
* Les standards de compilation sont les standards gnu99

L'entièreté des tests ont été effectué sur une machine ayant :
* un processeur intel core I5 9400f ayant 6 coeurs physiques
* 8Go de RAM DDR4 cadencées à 2666MHz

## **Important** : Interface des primitives 

Pour simplifier l'utilisation des trois algorithmes avec les différentes primitives de synchronisations, j'ai implémenté une interface générique permettant de choisir à l'aide d'un argument en ligne de commande quelle primitive utiliser lors de l'exécution de chaque programme. Ainsi donner l'argument "POSIX" au programme le fera utiliser les primitives POSIX alors que donner l'argument "TAS" ou "TATAS" permet d'utiliser les verrous à attente active *test-and-set* ou *test-and-test-and-set* 

J'ai donc mit en place une structure comprenant une énumération permettant de connaître le type de données souhaitées :
```c
typedef enum {
    POSIX,
    TAS,
    TATAS
} algo_t;
```

Cette énumération, couplée avec les unions présentées ci-dessous permet de définir des structures de données *mutex_t* et *semaphore_t* génériques, c'est à dire pouvant supporter plusieurs algorithmes différents.
### Interface des mutex

L'interface des mutex est mise en place à l'aide de la structure *mutex_t* définie comme suit :

```c
typedef struct {
  algo_t type;
  union {
    pthread_mutex_t* posix;
    spinlock_t* spinlock;
  };
} mutex_t;
```
L'entier ``type`` permet de déterminer quel type de mutex utiliser et l'union permet d'associer le type de mutex souhaité. Comme stipulé ci-dessus ``type = 0`` signifie que l'on souhaite utiliser un mutex POSIX, 1 si l'on souhaite un mutex TAS et 2 pour un mutex TATAS.

A cette structure est associée 4 fonctions, permettant de définir un *type abstrait de donnée*. Ces fonctions sont ``init_mutex(mutex_t*, algo_t algo)``, ``destroy_mutex(mutex_t*)``, ``lock(mutex_t*)``, ``unlock(mutex_t*)``.

La fonction d'initialisation prend un pointeur vers un ``mutex_t`` ainsi qu'un int et va permettre d'initialiser un mutex POSIX, TAS ou TATAS suivant la valeur de l'entier fourni. La fonction ``destroy_mutex()`` permet comme son nom l'indique de désallouer toutes les ressources utilisées par le ``mutex_t``.

Les fonctions ``lock()`` et ``unlock()`` sont les parties les plus importantes de cette interface. Elles prennent en argument un pointeur vers un ``mutex_t``, regardent la valeur de ``mutex_t->type`` et effectuent les appels aux fonctions de la librairie ``<pthread.h>`` si le mutex est de type POSIX ou aux fonctions de ``"primitives.h"`` si le mutex est de type TAS ou TATAS. 

### Interface des sémaphores

De la même manière, la structure *semaphore_t* comprend un *algo_t* et une *union* de cette forme :
```c
typedef struct {
    algo_t type;
    union{
        sem_t* posix;
        spinsem_t* spinsem;
    };
} semaphore_t;
```

Celle-ci permettant de définir les fonctions génériques ``init_semaphore(semaphore_t*, algo_t)``, ``destroy_semaphore(semaphore_t*)``n ``wait(semaphore_t*)`` et ``post(semaphore_t*)``.

Les fonctions ``wait()`` et ``post()`` observent la valeur ``type`` de la ``semaphore_t*`` fournie en argument afin de déterminer si il faut exécuter les fonctions de la librairie ``<semaphore.h>`` ou bien celles définies dans ``"primitives.h"``.


## Structure du projet :

Le projet contient de nombreux fichiers :
* ``benchmark`` : Contient les fichiers relatifs aux tests de performances. Il y'a donc le fichier bash d'automatisation des tests de performances, le fichier python permettant de plot les graphes ainsi que les données brutes au format ``.csv``
* ``report`` : Contient les fichiers ``.tex`` utilisé pour l'écriture du rapport.
* ``src`` : Contient tous les fichiers sources et headers nécessaires à la compilation des éxécutables.
  - ``cmnfunc`` : implémente les fonctions communes à plusieurs des exécutables.
  - ``primitives`` : implémente les primitives de synchronisation ainsi qu'une interface facilitant l'utilisation des différents paramètres (cette interface est décrite ci-dessus).
  - ``philosophes`` : implémente le problème des philosophes
  - ``prodcons`` : implémente le problème des producteurs-consommateurs
  - ``readwrt`` : implémente le problème des lecteurs-écrivains
  - ``spinlock`` : implémente les tests des primitives de synchronisation définies dans ``primitives``
* Le Makefile permettant d'automatiser les tâches de développements et de testing, ces cibles sont définies ci-dessous. 
* Ce README

## Cibles du Makefile

- (cible par défaut) ``all`` : compile tous les exécutables.
- ``benchmark`` : Effectue les tests de performances ainsi que les graphes.
- ``clean`` : Supprime les exécutables ainsi que les résidus de compilations.
- ``fullclean`` : effectue la cible ``clean`` et supprime également les graphes ainsi que les données des tests de performances.
 
Le Makefile contient d'autres cibles plus spécifiques permettant de compiler ou effectuer les tests de performances d'un seul programme, il ne sont néanmoins pas listés ici.