#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "options.h"

#include "sem.h"

#define WAITING_CHAIRS 5

typedef struct {
    int id;
    struct options opt;
} barber_args_t;

typedef struct {
    int id;
} client_args_t;

sem_t customers;
sem_t barbers;
sem_t mutex_waiting;
int waiting_clients = 0;
volatile int stop_barbers = 0;

void *barber_function(void *arg) {
    barber_args_t *args = (barber_args_t *)arg;
    int barber_id = args->id;
    struct options opt = args->opt;
    
    while (!stop_barbers) {
        sem_p(&customers);
        
        sem_p(&mutex_waiting);
        waiting_clients--;
        sem_v(&mutex_waiting);
        
        printf("Barber %d: cutting hair\n", barber_id);
        sleep(opt.cut_time / 1000); // Simulate time taken to cut hair
        printf("Barber %d: finished cutting hair\n", barber_id);
        
        sem_v(&barbers);
    }
    
    free(args);
    return NULL;
}

void *client_function(void *arg) {
    client_args_t *args = (client_args_t *)arg;
    int client_id = args->id;
    
    sem_p(&mutex_waiting);
    
    if (waiting_clients < WAITING_CHAIRS) {
        waiting_clients++;
        printf("Customer %d: sitting in the waiting room (waiting: %d)\n", client_id, waiting_clients);
        sem_v(&mutex_waiting);
        
        sem_v(&customers);
        sem_p(&barbers);
        
        printf("Customer %d: getting his hair cut\n", client_id);
        printf("Customer %d: leaving the barber shop\n", client_id);
    } else {
        printf("Customer %d: no chairs available, leaving\n", client_id);
        sem_v(&mutex_waiting);
    }
    
    free(args);
    return NULL;
}

int main (int argc, char **argv)
{
    struct options opt;

    // Default values for the options
    opt.barbers = 5;
    opt.customers = 1000;
    opt.cut_time  = 3000;

    read_options(argc, argv, &opt);
    printf("====INITIAL VALUES====\nBarbers: %d\nCustomers: %d\nCut Time: %d\n\n", opt.barbers, opt.customers, opt.cut_time);

    pthread_t barber_threads[opt.barbers];
    pthread_t client_threads[opt.customers];
    int i;

    sem_init(&customers, 0);
    sem_init(&barbers, 0);
    sem_init(&mutex_waiting, 1);

    for (i = 0; i < opt.barbers; i++) {
        barber_args_t *args = malloc(sizeof(barber_args_t));
        args->id = i + 1;
        args->opt = opt;
        pthread_create(&barber_threads[i], NULL, barber_function, args);
    }

    for (i = 0; i < opt.customers; i++) {
        client_args_t *args = malloc(sizeof(client_args_t));
        args->id = i + 1;
        pthread_create(&client_threads[i], NULL, client_function, args);
        sleep(rand() % 3); // Simulate time between customer arrivals
    }

    for (i = 0; i < opt.customers; i++) {
        pthread_join(client_threads[i], NULL);
    }

    stop_barbers = 1;

    sem_destroy(&customers);
    sem_destroy(&barbers);
    sem_destroy(&mutex_waiting);

    exit (0);
}
