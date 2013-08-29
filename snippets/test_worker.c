#include "thread.h"
#include "debug.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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

int g_flag;
static thread_pool  tp;                  /* instance of thread pool */
static thread_pool *pool = &tp;

pthread_mutex_t mutex;
pthread_mutex_t pool_mutex;
pthread_mutex_t io_mutex;
pthread_cond_t  cond;

int nthread;

typedef struct {
    thread_pool *pool;
    uint64_t     id;
} thread_argument;

#define WORKER_ENABLE 2

void thread_add_task(void *(*callback)(void *), void *arg)
{
    int i;

    pthread_mutex_lock(&pool_mutex);
    for (i = 0; i < WORKER_ENABLE; ++i) {
        if (pool->bitmap & (1 << i)) {
            dprintf(INFO, "%d busy\n", i);
            continue;
        }
        pool->tasks[i].routine = callback;
        pool->tasks[i].arg = arg;
        pool->bitmap |= (1 << i);
        assert(pool->bitmap & (1 << i));
        dprintf(INFO, "%d loaded\n", i);
        pthread_mutex_unlock(&pool_mutex);
        break;
    }
    pthread_cond_signal(&cond);
}

static void *thread_pool_wrapper(void *arg)
{
    thread_pool *pool = (thread_pool *)arg;

    uint64_t id = __sync_fetch_and_add(&nthread, 1);
    dprintf(WARN, "id: %lu\n", id);

    
    dprintf(INFO, "%lu idle: %lx\n", id, pthread_self());
    while (1) {
        pthread_cond_wait(&cond, &mutex);
        if (pool->tear_down) {
            pthread_mutex_unlock(&mutex);
            /* pthread_cond_signal(&cond); */
            pthread_exit(0);
        }

        /* need lock? */
        if (pool->tasks[id].routine) {
            dprintf(INFO, "%lu start\n", id);
            (*(pool->tasks[id].routine))(pool->tasks[id].arg);

            dprintf(INFO, "%lu finish\n", id);
            pthread_mutex_lock(&pool_mutex);
            pool->bitmap &= ~(1 << id);
            pool->tasks[id].routine = NULL;
            pool->tasks[id].arg = NULL;
            pthread_mutex_unlock(&pool_mutex);
        } else {
            pthread_t tid = pthread_self();
            dprintf(INFO, "hello, I'm %x. None of my biz.\n", tid);
        }
    }
}

void thread_pool_init()
{
    uint64_t i;

    pool->bitmap = 0L;
    pool->tear_down = 0;
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&pool_mutex, NULL);
    pthread_mutex_init(&io_mutex, NULL);
    pthread_cond_init(&cond, NULL);

    pthread_mutex_lock(&mutex);
    memset(&pool->tasks, 0, MAX_WORKER * sizeof(worker_task));
    for (i = 0; i < WORKER_ENABLE; ++i)
    {
        pthread_create(&(pool->tasks[i].tid), NULL, thread_pool_wrapper, pool);
        dprintf(INFO, "pthread_create: %x\n", pool->tasks[i].tid);
    }
    pthread_mutex_unlock(&mutex);
}

void thread_pool_destroy()
{
    uint8_t i;
    pool->tear_down = 1;
    pthread_cond_broadcast(&cond);
    for (i = 0; i < WORKER_ENABLE; ++i) {
        /* if (pool->bitmap & (1 << i)) { */
        /*     fprintf(stdout, "pthread_join: %d\n", i); */
            pthread_join(pool->tasks[i].tid, NULL);
        /* } */
    }
    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&io_mutex);
    pthread_mutex_destroy(&pool_mutex);
    pthread_mutex_destroy(&mutex);
}


void *simple(void *arg)
{
    while (g_flag == 0) {}
    fprintf(stdout, "easy!\n");
    return NULL;
}

int main(int argc, char *argv[])
{
    g_flag = 0;
    thread_pool_init();
    sleep(1);
    /* pthread_cond_broadcast(&cond); */
    thread_add_task(simple, NULL);
    thread_add_task(simple, NULL);
    sleep(1);
    g_flag = 1;
    sleep(1);
    thread_pool_destroy();
    fflush(stdout);
    return 0;
}
