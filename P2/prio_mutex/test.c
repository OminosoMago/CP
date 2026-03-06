#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include "prio_mutex.h"

struct args{
	int prio;
	prio_mutex_t *m;
};

void * f_prueba(void *ptr){
	struct args* arg=ptr;
	prio_mutex_lock(arg->m,arg->prio);
	printf("prioridad:%d\n",arg->prio);
	usleep(5000);
	prio_mutex_unlock(arg->m);
	return NULL;
}

int main(){
	prio_mutex_t *m=malloc(sizeof(prio_mutex_t));
	int n_prio=10;

	pthread_t prio[n_prio];
	
	struct args* arg[n_prio];

	printf("Inicializando mutex\n");
	prio_mutex_init(m,n_prio);

	printf("Inicializando argumentos\n");
	for(int i=0;i<n_prio;i++){
		arg[i]=malloc(sizeof(struct args));
		arg[i]->m=m;
		arg[i]->prio=i;
	}

	printf("Llamando a los threads\n");
	for(int i=0; i<n_prio;i++){
		pthread_create(&prio[i],NULL,f_prueba,arg[i]);
	}

	printf("Esperando a los threads\n");

	for(int i=0; i<n_prio;i++){
		pthread_join(prio[i],NULL);
	}
	
	printf("Borrando mutex\n");
	prio_mutex_destroy(m);
	free(m);

	printf("limpiando\n");
	for(int i=0; i<n_prio;i++){
		free(arg[i]);
	}
	
}
