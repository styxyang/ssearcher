#include "pool.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define WORKER_ENABLE 4

pthread_mutex_t mutex;
pthread_cond_t  cond;

void *thread_pool_wrapper(void *arg)
{
    pthread_cond_wait(&cond, &mutex);
    pthread_t tid = pthread_self();
    fprintf(stdout, "hello, I'm %x\n", tid);
    pthread_mutex_unlock(&mutex);    
    pthread_cond_signal(&cond);
    pthread_exit(0);
}

void thread_pool_init(thread_pool *pool)
{
    uint8_t i;

    pool->bitmap = 0L;
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    pthread_mutex_lock(&mutex);
    memset(&pool->tasks, 0, MAX_WORKER * sizeof(worker_task));
    for (i = 0; i < WORKER_ENABLE; ++i)
    {
        pthread_create(&(pool->tasks[i].tid), NULL, thread_pool_wrapper, NULL);
        fprintf(stdout, "pthread_create: %x\n", pool->tasks[i].tid);
    }
    pthread_mutex_unlock(&mutex);
}

void thread_pool_destroy(thread_pool *pool)
{
    uint8_t i;
    pthread_cond_broadcast(&cond);
    for (i = 0; i < WORKER_ENABLE; ++i) {
        if (pool->bitmap & (1 << i)) {
            fprintf(stdout, "pthread_join: %d\n", i);
            pthread_join(pool->tasks[i].tid, NULL);
        }
    }
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
}
int main(int argc, char *argv[])
{
    thread_pool tp;
    thread_pool_init(&tp);
    pthread_cond_signal(&cond);
    /* sleep(1); */
    thread_pool_destroy(&tp);
    return 0;
}
