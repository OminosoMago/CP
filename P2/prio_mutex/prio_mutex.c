#include <stdlib.h>
#include <pthread.h>

#include "prio_mutex.h"

int prio_mutex_init(prio_mutex_t *m, int num_prios) {
	m->mutex=malloc(sizeof(pthread_mutex_t));
	m->vector=malloc(sizeof(pthread_mutex_t));
	m->cond=malloc(sizeof(pthread_cond_t));
	pthread_mutex_init(m->mutex,NULL);
	pthread_mutex_init(m->vector,NULL);
	pthread_cond_init(m->cond,NULL);
	m->locked=0;
	m->n_prios=num_prios;
	m->prio_q=calloc(num_prios,sizeof(int));
	return 0;
}

int prio_mutex_destroy(prio_mutex_t *m) {
	pthread_mutex_destroy(m->mutex);
	pthread_mutex_destroy(m->vector);
	pthread_cond_destroy(m->cond);
	free(m->mutex);
	free(m->vector);
	free(m->cond);
	free(m->prio_q);
	return 0;
}

int is_highest_prio(prio_mutex_t *m,int prio){
	int flag=1;
	for(int i=(m->n_prios-1);i>prio;i--){
		if(m->prio_q[i]){
			flag=0;
			break;
		};
	}
	return flag;
}

int prio_mutex_lock(prio_mutex_t *m, int prio) {
        int res;
        pthread_mutex_lock(m->vector);
        m->prio_q[prio]++;
        while(m->locked || !is_highest_prio(m,prio)){
                pthread_cond_wait(m->cond,m->vector);
        }
	m->locked=1;
        m->prio_q[prio]--;
	res=pthread_mutex_lock(m->mutex);
        pthread_mutex_unlock(m->vector);
        return res;
};

int prio_mutex_unlock(prio_mutex_t *m) {
	int res;
	res=pthread_mutex_unlock(m->mutex);
	pthread_mutex_lock(m->vector);
	m->locked=0;
	pthread_cond_broadcast(m->cond);
	pthread_mutex_unlock(m->vector);
	return res;
};

int prio_mutex_trylock(prio_mutex_t *m) {
    return 0;
};
