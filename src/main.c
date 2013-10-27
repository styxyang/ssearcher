#include "config.h"
#include "options.h"
#include "match.h"
#include "debug.h"
#include "thread.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>

#include <pthread.h>
#include <sched.h>
#include <poll.h>

pthread_t pid[NTHR];
int pipefd[2];
int result[NCPU][2];

void sf_init()
{
    int i;

    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < NCPU; i++) {
        if (pipe(result[i]) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }
}

void sf_exit()
{
    int i;
    close(pipefd[0]);
}

int sf_check_buffer()
{
    int i;

    struct pollfd fdset[NCPU];
    memset(&fdset, 0, sizeof(fdset));

    for (i = 0; i < NCPU; ++i) {
        fdset[i].fd = result[i][0];
        fdset[i].events = POLLIN;
        fdset[i].revents = 0;
    }

    while (1) {
        static int cnt = 0;
        /* dprintf(WARN, "cnt: %d", cnt++); */
        int ret = poll(fdset, NCPU, 100);

        if (ret > 0) {
            if (fdset[0].revents & POLLIN) {
                char result[256];
                memset(result, 0, sizeof(result));
                int n = read(fdset[0].fd, result, sizeof(result));
                if (n == 0)
                    return 0;
                char buf[100000];
                setvbuf(stdout, buf, _IOFBF, sizeof(buf));
                printf("[%zu]%s\n", strlen(result), result);
                /* read from readfd, 
                   since you can read from it without being blocked */
            }
            /* if (fdset[1].revents & POLLOUT) { */
            /*     /\* write to writefd, */
            /*        since you can write to it without being blocked *\/ */
            /* } */
            if (fdset[0].revents & POLLERR)
                dprintf(ERROR, "POLLERR");
            if (fdset[0].revents & POLLHUP) {
                return -1;
            }
            if (fdset[0].revents & POLLNVAL)
                dprintf(ERROR, "POLLNVAL");
        } else if (ret == 0) {
            dprintf(ERROR, "ret = 0");
            /* return -1; */
            /* the poll has timed out, nothing can be read or written */
        } else {
            /* the poll failed */
            dprintf(ERROR, "ret < 0");
            perror("poll failed");
            return -1;
        }
        cpu_relax();
    }
}

void test_procon()
{
    pthread_mutex_init(&outmtx, NULL);
    pthread_mutex_init(&readmtx, NULL);
    pthread_create(&pid[0], NULL, dispatcher_thread, NULL);
    pthread_create(&pid[1], NULL, worker_thread, (void *)0);
    pthread_create(&pid[2], NULL, worker_thread, (void *)1);
    /* pthread_create(&pid[2], NULL, worker_thread, NULL); */
    /* check_buffer(); */
    pthread_join(pid[0], NULL);
    pthread_join(pid[1], NULL);
    pthread_join(pid[2], NULL);
    pthread_mutex_destroy(&readmtx);
    pthread_mutex_destroy(&outmtx);
}


int main(int argc, char *argv[])
{
    parse_options(argc, argv);
    sf_init();
    test_procon();
    sf_exit();
    return 0;
}
