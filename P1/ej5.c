#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#define I 6
#define WAIT_N 1000000

volatile int mon_sig=1;

struct data{
	volatile int* n;
	pthread_mutex_t *mutex;
};

struct arg{
	struct data* a;
	struct data* b;
};

void esperar_ns(long ns) {
    struct timespec ts;
    ts.tv_sec = ns / 1000000000;
    ts.tv_nsec = ns % 1000000000;
    nanosleep(&ts, NULL);
}

pthread_mutex_t *sel(pthread_mutex_t *a,pthread_mutex_t *b){
	return ((uintptr_t)a < (uintptr_t)b) ? a:b;
}

void *thread_a(void* ptr){
       	struct arg* arg=ptr;
	pthread_mutex_t *pri=sel(arg->a->mutex,arg->b->mutex);
	pthread_mutex_t *sec;
	if(pri==arg->a->mutex){
		sec=arg->b->mutex;
	}
	else{
		sec=arg->a->mutex;
	}
	for(int i=0;i<I;i++){
		pthread_mutex_lock(pri);
		pthread_mutex_lock(sec);
		*(arg->a->n)=*(arg->a->n)+1;
		*(arg->b->n)=*(arg->b->n)-1;
		pthread_mutex_unlock(pri);
		pthread_mutex_unlock(sec);
		esperar_ns(WAIT_N);
	}
}

void *thread_b(void* ptr){
	struct arg* arg=ptr;
	pthread_mutex_t *pri=sel(arg->a->mutex,arg->b->mutex);
	pthread_mutex_t *sec;
	if(pri==arg->a->mutex){
		sec=arg->b->mutex;
	}
	else{
		sec=arg->a->mutex;
	}
	for(int i=0;i<I;i++){
		pthread_mutex_lock(pri);
		pthread_mutex_lock(sec);
		*(arg->a->n)=*(arg->a->n)-1;
		*(arg->b->n)=*(arg->b->n)+1;
		pthread_mutex_unlock(pri);
		pthread_mutex_unlock(sec);
		esperar_ns(WAIT_N);
	}
}

void *thread_mon(void* ptr){
	struct arg* arg=ptr;
	pthread_mutex_t *pri=sel(arg->a->mutex,arg->b->mutex);
	pthread_mutex_t *sec;
	if(pri==arg->a->mutex){
		sec=arg->b->mutex;
	}
	else{
		sec=arg->a->mutex;
	}
	while(mon_sig){	
		pthread_mutex_lock(pri);
		pthread_mutex_lock(sec);
		printf("%d %d %d\n",*(arg->a->n),*(arg->b->n),*(arg->b->n)+*(arg->a->n));
		pthread_mutex_unlock(pri);
		pthread_mutex_unlock(sec);
		esperar_ns(WAIT_N);
	}
}
void crear_mutex(pthread_mutex_t **mut){
	pthread_mutex_init(*mut,NULL);
}

void borrar_mutex(pthread_mutex_t **mut){
	pthread_mutex_destroy(*mut);
}

void inicializar_data(
	struct arg* estructura,pthread_mutex_t* mut1,pthread_mutex_t* mut2){
	
	estructura->a=malloc(sizeof(struct data));
	estructura->b=malloc(sizeof(struct data));
	estructura->a->n=malloc(sizeof(int));
	estructura->b->n=malloc(sizeof(int));
	*(estructura->a->n)=0;
	*(estructura->b->n)=0;
	estructura->a->mutex=mut1;
	estructura->b->mutex=mut2;
}
void iniciar_vector(struct arg* v[],struct arg* elem,int n){
	for(int i=0;i<n;i++){
		v[i]=elem;
	}
}
void borrar_data(struct arg* estructura){
	free((void *)estructura->a->n);
	free((void *)estructura->b->n);
	free(estructura->a);
	free(estructura->b);
}
int main(){
	pthread_t thr_b;
	pthread_t thr_a;
	pthread_t thr_mon;
	
	pthread_mutex_t *mut1=malloc(sizeof(pthread_mutex_t));
	pthread_mutex_t *mut2=malloc(sizeof(pthread_mutex_t));

	crear_mutex(&mut1);
	crear_mutex(&mut2);

	struct arg* arg1=malloc(sizeof(struct arg));
	inicializar_data(arg1,mut1,mut2);

	struct arg arg2={
		arg1->a,
		arg1->b
	};

	pthread_create(&thr_mon,NULL,thread_mon,arg1);
	pthread_create(&thr_a,NULL,thread_a,arg1);
	pthread_create(&thr_b,NULL,thread_b,&arg2);
	pthread_join(thr_a,NULL);
	pthread_join(thr_b,NULL);
	mon_sig=0;
	pthread_join(thr_mon,NULL);

	borrar_data(arg1);
	free(arg1);

	borrar_mutex(&mut1);
	borrar_mutex(&mut2);
	free(mut1);
	free(mut2);	
}
