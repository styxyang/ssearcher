#include "config.h"
#include "debug.h"
#include "thread.h"
#include "magic.h"
#include "file.h"
#include "match.h"
#include "options.h"
#include "buffer.h"

#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <sched.h>
#include <pthread.h>
#ifdef ENABLE_FTS
#include <fts.h>
#endif

extern int pipefd[2];
extern int result[NCPU][2];
extern __thread size_t off;

__thread long tid;

pthread_mutex_t outmtx;
pthread_mutex_t readmtx;

static int32_t begin_of_line(char *p, int32_t mid)
{
    while (mid >= 0) {
        if (p[mid] == '\n')
            break;
        mid--;
    }
    return (mid + 1);
}

void *worker_thread(void *arg)
{
    ssize_t n, r;
    int     fd;
    tid = (long)arg;
    fileinfo fi;
    size_t  patlen = strlen(opt.search_pattern);

    {
        init_buffer();
        kmp_prepare(opt.search_pattern, patlen);
    }

    /* loop to read opened file descriptors from pipe */
    /* while ((n = read(pipefd[0], &fi, sizeof(struct fileinfo))) != 0) { */
    while (1) {
        pthread_mutex_lock(&readmtx);
        n = read(pipefd[0], &fi, sizeof(fileinfo));
        pthread_mutex_unlock(&readmtx);

        if (n == 0)
            break;

        if (n != sizeof(fileinfo)) {
            cpu_relax();
            continue;
        }

        char buf[16];
        char *p;
        if ((p = map_file(&fi)) == NULL) {
            close(fi.fd);
            cpu_relax();
            continue;
        }

        int32_t matchpos = 0, startpos = 0;
        uint32_t linum = 1;
        uint32_t lastline = 0;
        size_t nbuf = 0;

        /* FIXME opt.search_pattern should be guarenteed to be not null */
        while (1) {
            matchpos = kmp_match(p + startpos,
                                 fi.size - startpos,
                                 opt.search_pattern,
                                 patlen,
                                 &linum);
            /* if (matchpos < 0 || !inbound(matchpos)) */
            if (matchpos < 0)
                break;
            dprintf(INFO, "T%d <%s> match at %d", tid, fi.filename, matchpos);

            memset(buf, 0, sizeof(buf));
            int32_t bol;
            if ((bol = begin_of_line(p, matchpos + startpos)) >= 0) {
                static int cnt;
                if (linum != lastline) {
                    writef_buffer(LINUM_COLOR "\n%u:" "\e[0m", linum);
                    nbuf = writeline_color_buffer(p + bol, 1024, matchpos + startpos - bol, patlen);
                    lastline = linum;
                    cnt = 1;
                } else {
                    dprintf(WARN, "another match in the same line");
                    nbuf += amendline_color_buffer(nbuf, matchpos + startpos - bol, patlen, cnt);
                    cnt++;
                }
            }
            /* There should be no exceptions */

            /* dprintf(INFO, "write to result pipe\n"); */
            /* write(result[tid][1], buf, sizeof(buf)); */
            startpos += matchpos + patlen;
        }

        if (off) {
            /* FIXME abstract as `write_filename' maybe */
            pthread_mutex_lock(&outmtx);
            fprintf(stdout, FNAME_COLOR "\n%s" "\e[0m", fi.filename);
            fprintf(stdout, "%s\n", read_buffer());
            pthread_mutex_unlock(&outmtx);
            fflush(NULL);
        }

        reset_buffer();
        unmap_file(&fi);
        cpu_relax();
    }

    {
        destroy_buffer();
        kmp_finish();
        close(result[tid][1]);
        dprintf(INFO, "result[%ld][1] closed", tid);
    }

    return 0;
}

void *dispatcher_thread(void *arg)
{
    struct stat    statbuf;
    DIR           *d;
    struct dirent *entry;

    magic_init();

    dprintf(INFO, "dispatcher");
#ifndef ENABLE_FTS
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

            fileinfo fi;
            memset(&fi, 0, sizeof(fileinfo));
            fi.fd = fd;
            memcpy(fi.filename, entry->d_name, strlen(entry->d_name));
            /* if (write(pipefd[1], &fd, sizeof(int)) != sizeof(int)) { */
            /*     perror("write pipefd\n"); */
            /* } */
            if (write(pipefd[1], &fi, sizeof(fileinfo)) != sizeof(fileinfo)) {
                perror("write pipefd\n");
            }
        }
        cpu_relax();
    }

    if (closedir(d) < 0) {
        dprintf(ERROR, "closedir failed");
    }
#else
    char * const path_argv[] = { ".", NULL};
    FTSENT *fts_entry = NULL;
    FTS *pfts = fts_open(path_argv, FTS_LOGICAL, NULL);
    dprintf(INFO, "open current directory");
    while (fts_entry = fts_read(pfts)) {
        if (strcmp(fts_entry->fts_name, ".git") == 0) {
            fts_set(pfts, fts_entry, FTS_SKIP);
            goto next_entry;
        }

        if (strcmp(fts_entry->fts_name, ".svn") == 0) {
            fts_set(pfts, fts_entry, FTS_SKIP);
            goto next_entry;
        }

        if (strcmp(fts_entry->fts_name, ".hg") == 0) {
            fts_set(pfts, fts_entry, FTS_SKIP);
            goto next_entry;
        }

        if (!S_ISREG(fts_entry->fts_statp->st_mode)) {
            goto next_entry;
        }

        int fd;
        if ((fd = open(fts_entry->fts_path, O_RDONLY)) == -1) {
            dprintf(ERROR, "fail to open file for read: %s", fts_entry->fts_path);
            continue;
        }
        dprintf(INFO, "open file: %d %s\n", fd, fts_entry->fts_path);

        /* test whether the file is binary */
        /* and close binary file */
        if (magic_scan(fd)) {
            dprintf(WARN, "close binary %s", fts_entry->fts_path);
            close(fd);
            continue;
        }

        fileinfo fi;
        memset(&fi, 0, sizeof(fileinfo));
        fi.fd = fd;
        memcpy(fi.filename, fts_entry->fts_path, fts_entry->fts_pathlen);
        /* if (write(pipefd[1], &fd, sizeof(int)) != sizeof(int)) { */
        /*     perror("write pipefd\n"); */
        /* } */
        if (write(pipefd[1], &fi, sizeof(fileinfo)) != sizeof(fileinfo)) {
            perror("write pipefd\n");
        }
  next_entry:
        cpu_relax();
    }
    fts_close(pfts);
#endif  /* FTS */

    close(pipefd[1]);

    magic_fini();

    return 0;
}

