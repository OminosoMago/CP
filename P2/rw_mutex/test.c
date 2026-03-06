#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include "rw_mutex.h"

#define N_READERS 20
#define N_WRITERS 10

struct args{
	rw_mutex_t *m;
};

void * f_prueba_reader(void *ptr){
	struct args* arg=ptr;
	rw_mutex_readlock(arg->m);
	printf("Leyendo\n");
	usleep(5000);
	rw_mutex_readunlock(arg->m);
}

void * f_prueba_writer(void *ptr){
	struct args* arg=ptr;
	rw_mutex_writelock(arg->m);
	printf("Escribiendo\n");
	usleep(5000);
	rw_mutex_writeunlock(arg->m);
}

int main(){
	rw_mutex_t *m=malloc(sizeof(rw_mutex_t));

	pthread_t readers[N_READERS];
	pthread_t writers[N_WRITERS];
	
	struct args* arg_r[N_READERS];
	struct args* arg_w[N_WRITERS];

	printf("Inicializando mutex\n");
	rw_mutex_init(m);

	printf("Inicializando argumentos de readers\n");
	for(int i=0;i<N_READERS;i++){
		arg_r[i]=malloc(sizeof(struct args));
		arg_r[i]->m=m;
	}

	printf("Inicializando argumentos de writers\n");
	for(int i=0;i<N_WRITERS;i++){
		arg_w[i]=malloc(sizeof(struct args));
		arg_w[i]->m=m;
	}

	printf("Llamando a los threads\n");

	//Llama a los dos primeros threads(son tipo writer)
	pthread_create(&writers[0],NULL,f_prueba_writer,arg_w[0]);
	pthread_create(&writers[1],NULL,f_prueba_writer,arg_w[1]);
	
	//Llama a la mitad de los readers
	for(int i=0; i<N_READERS/2;i++){
		pthread_create(&readers[i],NULL,f_prueba_reader,arg_r[i]);
	}
	
	//Llama a todos los writers restantes
	for(int i=2; i<N_WRITERS;i++){
		pthread_create(&writers[i],NULL,f_prueba_writer,arg_w[i]);
	}

	//Llama a los readers restantes
	for(int i=N_READERS/2; i<N_READERS;i++){
		pthread_create(&readers[i],NULL,f_prueba_reader,arg_r[i]);
	}

	//Espera por los readers restantes
	for(int i=0; i<N_READERS;i++){
		pthread_join(readers[i],NULL);
	}

	//Espera por los writers restantes
	for(int i=0; i<N_WRITERS;i++){
		pthread_join(writers[i],NULL);
	}

	printf("Borrando mutex\n");
	rw_mutex_destroy(m);
	free(m);

	printf("limpiando\n");
	for(int i=0; i<N_READERS;i++){
		free(arg_r[i]);
	}

	for(int i=0; i<N_WRITERS;i++){
		free(arg_w[i]);
	}
	
}

