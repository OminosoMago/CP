#include <stdlib.h>
#include <pthread.h>

#include "sem.h"

int sem_init(sem_t *s, int value) {
	s->sem=malloc(sizeof(pthread_mutex_t));
	s->cond=malloc(sizeof(pthread_cond_t));
	pthread_mutex_init(s->sem,NULL);
	s->cont=0;
	return 1;
}

int sem_destroy(sem_t *s) {
	pthread_mutex_destroy(s->sem);
	free(s->sem);
	return 1;
}

int sem_p(sem_t *s) {
	pthread_mutex_lock(s->sem);
	while(s->cont==0){
		pthread_cond_wait(s->cond,s->sem);
	}
	s->cont--;
	pthread_mutex_unlock(s->sem);
	return 1;
}

int sem_v(sem_t *s) {
	pthread_mutex_lock(s->sem);
	s->cont++;
	pthread_cond_signal(s->cond);
	pthread_mutex_unlock(s->sem);
	return 1;
}

int sem_tryp(sem_t *s) { // 0 on sucess, -1 if already locked
	int res;
	pthread_mutex_lock(s->sem);
	if(s->cont==0){
		res=-1;
	}
	else{
		s->cont--;
		res=0;
	}
	pthread_mutex_unlock(s->sem);
	return res;
}
