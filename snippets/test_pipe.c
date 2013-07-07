#include "worker.h"
#include "debug.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>                      /* pthread_yield missing on OSX */

int fd[2];

pthread_t tid[2];

void *readpipe(void *arg)
{
    char buf[16];
    while (read(fd[0], buf, 12) == 12) {
        printf("%s", buf);
        memset(buf, 0, sizeof(buf));
        sched_yield();
    }
    pthread_exit(0);
}

void *writepipe(void *arg)
{
    int i = 0;
    while (i++ < 10) {
        write(fd[1], "hello world\n", 12);
        sched_yield();
    }
    write(fd[1], "\t", 1);
    pthread_exit(0);
}

int main(int argc, char *argv[])
{
    if (pipe(fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    pthread_create(&tid[0], NULL, readpipe, NULL);
    pthread_create(&tid[1], NULL, writepipe, NULL);
    printf("%d %d\n", fd[0], fd[1]);
    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);
    close(fd[0]);
    close(fd[1]);
    return 0;
}
