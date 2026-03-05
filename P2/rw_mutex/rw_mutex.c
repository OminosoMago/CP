#include <stdlib.h>
#include <pthread.h>

#include "rw_mutex.h"

int rw_mutex_init(rw_mutex_t *m){
	m->writers=malloc(sizeof(pthread_mutex_t));
	m->readers=malloc(sizeof(pthread_mutex_t));
	m->cond=malloc(sizeof(pthread_cond_t));
	pthread_mutex_init(m->writers,NULL);
	pthread_mutex_init(m->readers,NULL);
	pthread_cond_init(m->cond,NULL);
	m->n_readers=0;
}

int rw_mutex_destroy(rw_mutex_t *m) {
	pthread_mutex_destroy(m->writers);
	pthread_mutex_destroy(m->readers);
	pthread_cond_destroy(m->cond);
	free(m->writers);
	free(m->readers);
	free(m->cond);
}

int rw_mutex_readlock(rw_mutex_t *m) {
	pthread_mutex_lock(m->readers);
	m->n_readers++;
	if(m->n_readers==1){
		pthread_mutex_lock(m->writers);
	}
	pthread_mutex_unlock(m->readers);
}

int rw_mutex_writelock(rw_mutex_t *m) {
	pthread_mutex_lock(m->writers);
}

int rw_mutex_readunlock(rw_mutex_t *m) {
	pthread_mutex_lock(m->readers);
	m->n_readers--;
	if(m->n_readers==0){
		pthread_mutex_unlock(m->writers);
	}
	pthread_mutex_unlock(m->readers);
}

int rw_mutex_writeunlock(rw_mutex_t *m) {
	pthread_mutex_unlock(m->writers);
}
