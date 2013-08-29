#include "thread.h"
#include "debug.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dirent.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <unistd.h>
#include <pthread.h>
#include <sched.h>                      /* pthread_yield missing on OSX */

int fd0[2];
int fd1[2];

pthread_t tid[2];
pthread_t disp;

pthread_mutex_t g_mtx;

void *readpipe(void *arg)
{
    static int nthreads = 0;
    int id = __sync_fetch_and_add(&nthreads, 1);
    /* int *fd = (id == 0)?fd0:fd1; */
    int *fd = fd0;

    int buf;
    struct stat st;
    printf("thread %d\n", id);
    while (1) {
        /* pthread_mutex_lock(&g_mtx); */
        int n = read(fd[0], &buf, sizeof(int));
        /* pthread_mutex_unlock(&g_mtx); */
        if (n == sizeof(int)) {
            printf("%d-%d\n", id, buf);
            /* memset(&buf, 0, sizeof(int)); */
            fstat(buf, &st);
            printf("st_ino: %lld\n", st.st_ino);
            close(buf);
        } else if (n == 1 && buf == 0) {
            break;
        } else if (n == 0) {
            break;
        }
        /* sched_yield(); */
        cpu_relax();
    }
    pthread_exit(0);
}

void *dispatcher(void *arg)
{
    struct stat    statbuf;
    DIR           *d;
    struct dirent *entry;

    dprintf(INFO, "dispatcher");
    if ((d = opendir("./")) == NULL) {
        fprintf(stdout, "opendir failed\n");
        pthread_exit(0);
    }

    dprintf(INFO, "open dir: .");
    while ((entry = readdir(d)) != NULL) {
        struct stat st;
        char fullpath[512];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", ".", entry->d_name);
        lstat(fullpath, &st);
        if (S_ISREG(st.st_mode)) {
            dprintf(INFO, "%s", entry->d_name);

            /* pasted from main() */
            int fd;
            if ((fd = open(fullpath, O_RDONLY)) == -1) {
                dprintf(ERROR, "fail to open file for read: %s", fullpath);
                pthread_exit(0);
            }
            
            dprintf(INFO, "open file: %s\n", fullpath);
            if (write(fd0[1], &fd, sizeof(int)) != sizeof(int)) {
                perror("write fd0\n");
                /* if (write(fd1[1], &fd, sizeof(int)) != sizeof(int)) { */
                /*     perror("write fd1\n"); */
                /* } */
            }
            sched_yield();
        }
    }
    write(fd0[1], "\0", 1);
    write(fd0[1], "\0", 1);
    /* write(fd1[1], "\0", 1); */
    close(fd0[1]);
    close(fd1[1]);

    if (closedir(d) < 0) {
        dprintf(ERROR, "closedir failed");
        pthread_exit(0);
    }
    pthread_exit(0);
}

int main(int argc, char *argv[])
{
    pthread_mutex_init(&g_mtx, NULL);
    if (pipe(fd0) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    if (pipe(fd1) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    pthread_create(&tid[0], NULL, readpipe, NULL);
    pthread_create(&tid[1], NULL, readpipe, NULL);
    pthread_create(&disp, NULL, dispatcher, NULL);

    pthread_join(disp, NULL);
    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);
    close(fd0[0]);
    /* close(fd0[1]); */
    close(fd1[0]);
    /* close(fd1[1]); */
    pthread_mutex_destroy(&g_mtx);
    return 0;
}

