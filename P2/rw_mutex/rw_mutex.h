#ifndef __RW_MUTEX_H__
#define __RW_MUTEX_H__

#include <pthread.h>

typedef 
struct rw_mutex_t {
	pthread_mutex_t *writers;
	pthread_mutex_t *readers;
	pthread_cond_t *cond;
	int n_readers;
}rw_mutex_t;

int rw_mutex_init(rw_mutex_t *m);
int rw_mutex_destroy(rw_mutex_t *m);

int rw_mutex_readlock(rw_mutex_t *m);
int rw_mutex_writelock(rw_mutex_t *m);
int rw_mutex_readunlock(rw_mutex_t *m);
int rw_mutex_writeunlock(rw_mutex_t *m);

#endif
