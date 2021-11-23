# concurren-C

Ce projet contient de nombreux fichiers permettant d'analyser les performances de plusieurs algorithmes classiques de synchronisations :
* Le problème des philosophes
* Le problème des producteurs-consommateurs
* Le problème des lecteurs-écrivains

Nous allons tester ces trois algorithmes en fonction de deux paramètres : le nombre de thread et les primitives de synchronisation utilisées.
Le nombre de thread est un entier compris entre 1 et 2n où n est le nombre de coeurs du processeurs utilisé. Les primitives de synchronisations sont les mutex et sémaphores POSIX ou les verrous à attente active et sémaphores construit sur base de ces verrous.

Pour simplifier l'utilisation des trois algorithmes avec différentes primitives. J'ai implémenté une interface permettant de choisir quelles primitives utiliser. Ainsi donner l'argument "POSIX" au programme le fera utiliser les primitives POSIX alors que donner l'argument "TAS" ou "TATAS" permet d'utiliser les verrous à attente active *test-and-set* ou *test-and-test-and-set* 

L'interface des mutex est mise en place à l'aide de la structure *mutex_t* et des fonctions ``init_mutex()``, ``destroy_mutex()``, ``lock()``, ``unlock()``.

La structure *mutex_t* comprend un *int* exprimant le type de mutex utilisé (POSIX, TAS ou TATAS) ainsi qu'une *union* contenant soit un *pthread_mutex_t* soit un *spinlock_t* (défini dans ``primitives.h``). De cette manière, on peut définir une fonction générique ``lock()`` et ``unlock()`` prenant un *mutex_t* en entrée et effectuant l'opération souhaitée peu importe le type de mutex utilisé.

De la même manière, la structure *semaphore_t* comprend un *int* et une *union* permettant de définir les fonctions génériques ``wait()`` et ``post()``

## Structure du projet :

Le projet contient de nombreux fichiers :
* ``benchmark`` : Contient les fichiers relatifs aux tests de performances.
* ``benchmark/Data`` : Contient toutes les performances sous format ``.csv``
* ``src`` : Contient tous les fichiers sources et headers nécéssaires à la compilation des éxécutables.
  - ``cmnfunc`` : implémente les fonctions communes à plusieurs des exécutables.
  - ``primitives`` : implémente les primitives de synchronisation ainsi qu'une interface facilitant l'utilisation des différents paramètres (cette interface est décrite ci-dessus).
  - ``philosophes`` : implémente le problème des philosophes
  - ``prodcons`` : implémente le problème des producteurs-consommateurs
  - ``readwrt`` : implémente le problème des lecteurs-écrivains
  - ``spinlock`` : implémente les tests des primitives de synchronisation définies dans ``primitives``
* Un Makefile consistant en plusieurs targets :
  - (cible par défaut) ``all`` : compile tous les exécutables.
  - ``benchmark`` : Effectue les tests de performances ainsi que les graphes.
  - ``clean`` : Supprime les exécutables ainsi que les résidus de compilations.
  - ``fullclean`` : effectue la cible ``clean`` et supprime également les graphes ainsi que les données des tests de performances.