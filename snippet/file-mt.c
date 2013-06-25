#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "../src/config.h"

#defne MAX_THR 16

pthread_t tid[2];

void *thread_routine(void *targ)
{
    char *p = (char *)targ;
    printf ("%s\n", p);
    return NULL;
}

int pick_idle_thread() {
    uint64_t i;
    for (i = 0; i < MAX_THR; i++) {

    }
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf ("%s\n", "wrong number of input");
        exit(0);
    }

    if (argc > 16) {
        printf ("%s\n", "Too many files to be processed");
        exit(0);
    }

    char **filelist = argv + 1;
    int i;
    for (i = 0; i < argc - 1; i++) {
        pthread_create(&tid[i], NULL, thread_routine, filelist[i]);
        pthread_join(tid[i], NULL);
    }
    return 0;
}
