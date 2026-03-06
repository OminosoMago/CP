#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include "rec_mutex.h"

struct args {
    int id;
    int cont;       // número de locks recursivos a hacer
    rec_mutex_t *m;
};

void *f_prueba(void *ptr) {
    struct args *arg = ptr;

    printf("Thread %d: intentando lock (número de locks=%d)\n", arg->id, arg->cont);

    for (int i = 0; i < arg->cont; i++) {
        rec_mutex_lock(arg->m);
        printf("Thread %d: lock %d adquirido\n", arg->id, i + 1);
    }
    usleep(5000);
    for (int i = 0; i < arg->cont; i++) {
        rec_mutex_unlock(arg->m);
        printf("Thread %d: unlock %d hecho\n", arg->id, i + 1);
    }

    return NULL;
}

int main() {
    rec_mutex_t *m = malloc(sizeof(rec_mutex_t));
    int n_threads = 5;
    int cont = 3;
    pthread_t threads[n_threads];
    struct args *arg[n_threads];

    printf("Inicializando mutex recursivo\n");
    rec_mutex_init(m);

    printf("Inicializando argumentos\n");
    for (int i = 0; i < n_threads; i++) {
        arg[i] = malloc(sizeof(struct args));
        arg[i]->m = m;
        arg[i]->id = i;
        arg[i]->cont = cont;
    }

    printf("Llamando a los threads\n");
    for (int i = 0; i < n_threads; i++) {
        pthread_create(&threads[i], NULL, f_prueba, arg[i]);
    }

    printf("Esperando a los threads\n");
    for (int i = 0; i < n_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Borrando mutex\n");
    rec_mutex_destroy(m);
    free(m);

    printf("Limpiando\n");
    for (int i = 0; i < n_threads; i++) {
        free(arg[i]);
    }

    return 0;
}