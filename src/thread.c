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

/* thread id */
__thread long tid;

pthread_mutex_t outmtx;
pthread_mutex_t readmtx;

/* find the beginning offset of the line which contains
 * the `mid' position */
static uint32_t begin_of_line(char *fb, uint32_t mid)
{
    while (mid > 0) {
        if (fb[mid - 1] == '\n')
            break;
        mid--;
    }
    return mid;
}

/* write matched data to buffer with line number `linum' and `lastlinum' */
static void worker_writebuffer(char *fb, uint32_t pos,
                               uint32_t linum, uint32_t lastlinum)
{
    static size_t nll = 0; /* `nll' number of chars in last line */
    static int cnt;        /* `cnt' number of matches in this line */

    uint32_t bol = begin_of_line(fb, pos);
    if (linum != lastlinum) {
        writef_buffer(LINUM_COLOR "\n%u:" "\e[0m", linum);
        nll = writeline_color_buffer(fb + bol, 1024, pos - bol, opt.search_patlen);
        lastlinum = linum;
        cnt = 1;
    } else {
        dprintf(WARN, "another match in the same line");
        nll += amendline_color_buffer(nll, pos - bol,
                                      opt.search_patlen, cnt);
        cnt++;
    }
}

void *worker_thread(void *arg)
{
    ssize_t n, r;
    int     fd;
    tid = (long)arg;
    fileinfo fi;

    /* init block */
    {
        init_buffer();
        kmp_prepare(opt.search_pattern, opt.search_patlen);
    }

    /* loop to read opened file descriptors from pipe */
    while (1) {
        /* FIXME use queue to replace the pipe */
        pthread_mutex_lock(&readmtx);
        n = read(pipefd[0], &fi, sizeof(fi));
        pthread_mutex_unlock(&readmtx);

        if (n == 0)
            break;
        if (n != sizeof(fi)) {
            cpu_relax();
            continue;
        }

        char buf[16];
        char *fb;  /* `fb' for file buffer */
        if ((fb = map_file(&fi)) == NULL) {
            close(fi.fd);
            cpu_relax();
            continue;
        }

        /* `matchpos': the position matched in every iteration
         * `startpos': the position where every iteration starts */
        uint32_t matchpos = 0, startpos = 0;

        /* `linum': line number of every match result, initial line number is 1
         * 'lastlinum': line number of last match, used to mark matches in 
         *              the same line */
        uint32_t linum = 1;
        uint32_t lastlinum = 0;

        /* FIXME opt.search_pattern should be guarenteed to be not null */
        /* XXX shall we use likely/unlikely to do branch prediction? */
        while (1) {
            if (!kmp_match(fb + startpos, fi.size - startpos,
                                    &linum, &matchpos)) {
                break;
            }

            dprintf(INFO, "T%d <%s> match at %d", tid, fi.filename,
                    matchpos + startpos);

            memset(buf, 0, sizeof(buf));
            worker_writebuffer(fb, matchpos + startpos, linum, lastlinum);
            /* There should be no exceptions */

            /* dprintf(INFO, "write to result pipe\n"); */
            /* write(result[tid][1], buf, sizeof(buf)); */
            startpos += matchpos + opt.search_patlen;
        }

        /* if offset is not zero, buffer contains something to write */
        if (off) {
            /* FIXME abstract as `write_filename' maybe */
            pthread_mutex_lock(&outmtx);
            fprintf(stdout, FNAME_COLOR "%s" "\e[0m\n", fi.filename);
            if (!opt.list_matching_files)
                fprintf(stdout, "%s\n", read_buffer());
            pthread_mutex_unlock(&outmtx);
            free(fi.filename);
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
            /* memcpy(fi.filename, entry->d_name, strlen(entry->d_name)); */
            /* on OS X, should use `strndup(entry->d_name, entry->d_namelen)' */
            fi.filename = strdup(entry->d_name);
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
    while ((fts_entry = fts_read(pfts))) {
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
        /* memcpy(fi.filename, fts_entry->fts_path, fts_entry->fts_pathlen); */
        /* on OS X, should use `strndup(entry->d_name, entry->d_namelen)' */
        fi.filename = strndup(fts_entry->fts_path, fts_entry->fts_pathlen);
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

