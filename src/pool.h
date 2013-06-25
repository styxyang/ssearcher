#ifndef SS_POOL_H_
#define SS_POOL_H_

#include "../src/config.h"

#include <pthread.h>

#define MAX_WORKER 64

typedef struct
{
    void *(*routine)(void *arg);        /* worker callback routine */
    void *arg;                          /* argument to the callback */
    uint64_t  id;
    pthread_t tid;
} worker_task;

typedef struct {
    uint64_t bitmap;                    /* mask of which workers has been used */
    worker_task tasks[MAX_WORKER];
    uint64_t tear_down;
} thread_pool;


void thread_pool_init();
void thread_pool_destroy();
void thread_add_task(void *(*callback)(void *), void *arg);


#endif /* SS_POOL_H_ */