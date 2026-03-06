#include <stdlib.h>
#include <pthread.h>

#include "rec_mutex.h"

int rec_mutex_init(rec_mutex_t *m) {
	m->mutex = malloc(sizeof(pthread_mutex_t));
	m->owner_thread = malloc(sizeof(pthread_t));
	pthread_mutex_init(m->mutex, NULL);
	*m->owner_thread = 0;
	m->rec_count = 0;
	return 0;
}

int rec_mutex_destroy(rec_mutex_t *m) {
	pthread_mutex_destroy(m->mutex);
	free(m->mutex);
	free(m->owner_thread);
	return 0;
}

int rec_mutex_lock(rec_mutex_t *m) {
	pthread_t actual = pthread_self();

	pthread_mutex_lock(m->mutex);
	if (*m->owner_thread != actual || m->rec_count == 0) {
		*m->owner_thread = actual;
		m->rec_count = 1;
	} else {
		m->rec_count++;
	}
	pthread_mutex_unlock(m->mutex);
	return 0;
};

int rec_mutex_unlock(rec_mutex_t *m) {
	pthread_t actual = pthread_self();

	pthread_mutex_lock(m->mutex);
	if (m->rec_count == 0 && *m->owner_thread == actual) {
		m->rec_count--;
		if (m->rec_count == 0)
			m->owner_thread = 0;
	}
	pthread_mutex_unlock(m->mutex);
	return 0;
};

int rec_mutex_trylock(rec_mutex_t *m) {
    pthread_t actual = pthread_self();

	if (pthread_mutex_trylock(m->mutex) != 0) {
        return -1;
    }
	if (*m->owner_thread == actual || m->rec_count == 0) {
        if(m->rec_count == 0) {
            *m->owner_thread = actual;
        }
		m->rec_count++;
        pthread_mutex_unlock(m->mutex);
        return 0;
	}
	pthread_mutex_unlock(m->mutex);
	return -1; 
};
