#ifndef SS_POOL_H_
#define SS_POOL_H_

#include "ss_config.h"
#include <pthread.h>

#define ENABLE_FTS

/* Pause instruction to prevent excess processor bus usage */
#ifdef USE_PAUSE
#define cpu_relax()                             \
    do {                                        \
        asm volatile("pause\n": : :"memory");   \
    } while (0)
#else
#define cpu_relax()                             \
    do {                                        \
        sched_yield();                          \
    } while (0)
#endif

void *ss_dispatcher_thread(void *);
void *ss_worker_thread(void *arg);

extern pthread_mutex_t outmtx;
extern pthread_mutex_t readmtx;

#endif /* SS_POOL_H_ */
