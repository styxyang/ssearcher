#include "pool.h"
#include "../src/debug.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int g_flag;

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
