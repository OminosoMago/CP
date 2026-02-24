struct prio_mutex_t {
};

#include "prio_mutex.h"

int prio_mutex_init(prio_mutex_t *m, int num_prios) {
    return 0;
}

int prio_mutex_destroy(prio_mutex_t *m) {
    return 0;
}

int prio_mutex_lock(prio_mutex_t *m, int prio) {
    return 0;
};

int prio_mutex_unlock(prio_mutex_t *m) {
    return 0;
};

int prio_mutex_trylock(prio_mutex_t *m) {
    return 0;
};
