#include "pool.h"
#include "debug.h"

#include <assert.h>
#include <string.h>

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

