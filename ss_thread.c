#include "ss_config.h"
#include "ss_debug.h"
#include "ss_thread.h"
#include "ss_magic.h"
#include "ss_file.h"
#include "ss_match.h"
#include "ss_options.h"
#include "ss_buffer.h"

#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <sched.h>
#include <pthread.h>

extern int pipefd[2];
extern int ss_result[NCPU][2];
extern __thread size_t off;

pthread_mutex_t outmtx;

static int32_t begin_of_line(char *p, int32_t mid)
{
    while (mid >= 0) {
        if (p[mid] == '\n')
            return mid + 1;
        mid--;
    }
    if (mid != 0)
        return -1;
    return 0;
}

void *ss_worker_thread(void *arg)
{
    ssize_t n, r;
    int     fd;
    long    tid = (long)arg;
    struct fileinfo fi;

    init_buffer();

    /* loop to read opened file descriptors from pipe */
    /* while ((n = read(pipefd[0], &fi, sizeof(struct fileinfo))) != 0) { */
    while (1) {
        pthread_mutex_lock(&outmtx);
        n = read(pipefd[0], &fi, sizeof(struct fileinfo));
        pthread_mutex_unlock(&outmtx);
        if (n == 0)
            break;
        if (n != sizeof(struct fileinfo)) {
            cpu_relax();
            continue;
        }
        fd = fi.fd;

        char buf[16];
        char *p;
        if ((p = map_file(fd)) == NULL) {
            close(fd);
            cpu_relax();
            continue;
        }

        int32_t lastpos = 0, nextpos = 0, linum = 0;
        char ival[24];

        /* FIXME opt.search_pattern should be guarenteed to be not null */
        while (1) {
            lastpos = kmp_match(p + nextpos,
                                map_len(fd) - nextpos,
                                opt.search_pattern,
                                strlen(opt.search_pattern),
                                &linum);
            if (lastpos < 0 || !inbound(lastpos))
                break;

            memset(buf, 0, sizeof(buf));
            int32_t bol;
            if ((bol = begin_of_line(p, lastpos + nextpos)) >= 0) {
                dprintf(INFO, "write to result buffer");

                /* FIXME abstract as `write_filename' maybe */
                memset(ival, 0, sizeof(ival));
                sprintf(ival, LINUM_COLOR "%d:" "\e[0m", linum);
                write_buffer(ival, strlen(ival));

                writeline_color_buffer(p + bol, 100, lastpos + nextpos - bol, strlen(opt.search_pattern));
                write_buffer("\n", 1);
            }
            /* There should be no exceptions */

            /* dprintf(INFO, "write to result pipe\n"); */
            /* write(ss_result[tid][1], buf, sizeof(buf)); */
            nextpos += lastpos + strlen(opt.search_pattern);
        }

        if (off) {
            fprintf(stdout, FNAME_COLOR "%s" "\e[0m", fi.filename);
            fprintf(stdout, "\n%s\n", read_buffer());
            fflush(NULL);
        }

        reset_buffer();
        unmap_file(p);
        close(fd);
        cpu_relax();
    }

    destroy_buffer();

    close(ss_result[tid][1]);
    dprintf(INFO, "ss_result[%ld][1] closed", tid);
    return 0;
}

void *ss_dispatcher_thread(void *arg)
{
    struct stat    statbuf;
    DIR           *d;
    struct dirent *entry;

    magic_init();

    dprintf(INFO, "dispatcher");
    if ((d = opendir("./")) == NULL) {
        fprintf(stdout, "opendir failed\n");
        return 0;
    }

    dprintf(INFO, "open dir: .");
    while ((entry = readdir(d)) != NULL) {
        struct stat st;
        char fullpath[512];
        dprintf(INFO, "filename: %s", entry->d_name);
        snprintf(fullpath, sizeof(fullpath), "%s/%s", ".", entry->d_name);
        lstat(fullpath, &st);
        if (S_ISREG(st.st_mode)) {
            dprintf(INFO, "%s", entry->d_name);

            /* pasted from main() */
            int fd;
            if ((fd = open(fullpath, O_RDONLY)) == -1) {
                dprintf(ERROR, "fail to open file for read: %s", fullpath);
                return 0;
            }
            
            dprintf(INFO, "open file: %d %s\n", fd, fullpath);

            /* test whether the file is binary */
            /* and close binary file */
            if (magic_scan(fd)) {
                dprintf(WARN, "close binary %s", fullpath);
                close(fd);
                continue;
            }

            struct fileinfo fi;
            memset(&fi, 0, sizeof(struct fileinfo));
            fi.fd = fd;
            memcpy(fi.filename, entry->d_name, strlen(entry->d_name));
            /* if (write(pipefd[1], &fd, sizeof(int)) != sizeof(int)) { */
            /*     perror("write pipefd\n"); */
            /* } */
            if (write(pipefd[1], &fi, sizeof(struct fileinfo)) != sizeof(struct fileinfo)) {
                perror("write pipefd\n");
            }
        }
        cpu_relax();
    }

    close(pipefd[1]);

    if (closedir(d) < 0) {
        dprintf(ERROR, "closedir failed");
    }

    magic_fini();

    return 0;
}

