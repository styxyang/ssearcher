#include "ss_thread.h"
#include "ss_debug.h"

#include <assert.h>
#include <string.h>

#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>

extern int pipefd[2];

#define cpu_relax()                             \
    do {                                        \
        asm volatile("pause\n": : :"memory");   \
    } while (0)

void *ss_worker_thread(void *arg)
{
    ssize_t n;
    int     fd;

    /* loop to read opened file descriptors from pipe */
    while ((n = read(pipefd[0], &fd, sizeof(int))) != 0) {
        if (n != sizeof(int)) {
            cpu_relax();
            continue;
        }

        char buf[16];
        memset(buf, 0, sizeof(buf));
        read(fd, buf, sizeof(buf) - 1);
        dprintf(INFO, "%s\n", buf);
        close(fd);
    }

    pthread_exit(0);
}

void *ss_dispatcher_thread(void *arg)
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
            if (write(pipefd[1], &fd, sizeof(int)) != sizeof(int)) {
                perror("write pipefd\n");
            }
            cpu_relax();
        }
    }

    close(pipefd[1]);

    if (closedir(d) < 0) {
        dprintf(ERROR, "closedir failed");
        pthread_exit(0);
    }
    pthread_exit(0);
}

