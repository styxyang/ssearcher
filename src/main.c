#include "config.h"
#include "options.h"
#include "match.h"
#include "debug.h"

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

/* #define TEST_KMP */
#define TEST_DIR

pthread_t pid[2];
int finish[2] = { 0, 0 };
pthread_t mainthread;

#ifdef USE_PTHREAD
void myftw(const char *dirname, void *(*fn)(void *))
#else
void myftw(const char *dirname, int (*fn)(char *, int, char *, int))
#endif
{
    struct stat    statbuf;
    DIR           *d;
    struct dirent *entry;

    if (lstat(dirname, &statbuf) < 0) { /* stat error */
        dprintf(ERROR, "lstat error");
        exit(-1);
    }

    if (S_ISDIR(statbuf.st_mode) == 0) { /* not a directory */
        dprintf(ERROR, "target not a directory");
        exit(-1);
    }

    if ((d = opendir(dirname)) == NULL) {
        dprintf(ERROR, "opendir failed");
        exit(-1);
    }

    /* iteratively read directory entries */
    while ((entry = readdir(d)) != NULL) {
        struct stat st;
        char fullpath[512];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", dirname, entry->d_name);
        lstat(fullpath, &st);
        if (S_ISREG(st.st_mode)) {
            printf("%s\n", entry->d_name);

            /* pasted from main() */
            int fd;
            if ((fd = open(fullpath, O_RDONLY)) == -1) {
                dprintf(ERROR, "fail to open file for read: %s", fullpath);
                exit(-1);
            }

            /* map the file to a space of 4M bytes */
            char *p;
            if ((p = mmap(0, st.st_size, PROT_READ, MAP_SHARED, fd, 0)) == MAP_FAILED) {
                dprintf(ERROR, "ss: fail to map the file");
                exit(-1);
            }

            char *pattern = opt.search_pattern;

            int text_len, pat_len;

            pat_len = strlen(pattern);
            /* if (p[4095]) */
            /*     text_len = 4096; */
            /* else */
            /*     text_len = strlen(p); */
            text_len = st.st_size;

            dprintf(INFO, "text_len: %d, pat_len: %d\n", text_len, pat_len);
#ifdef USE_PTHREAD
            mainthread = pthread_self();
#endif

            int pos = 0;
            while (pos < st.st_size) {
#ifdef USE_PTHREAD
                /* thread_arg arg = { */
                /*     .pstart = p + pos, */
                /*     .text_len = text_len - pos, */
                /*     .pattern = pattern, */
                /*     .pat_len = pat_len, */
                /*     .result = -1, */
                /* }; */
                if (pid[0] == 0) {
                    thread_arg arg = {
                        .pstart = p + pos,
                        .text_len = text_len - pos,
                        .pattern = pattern,
                        .pat_len = pat_len,
                        .num = 0,
                        .result = -1,
                    };
                    pthread_create(&pid[0], NULL, fn, &arg);
                    dprintf(INFO, "thread %u starts on 0\n", pid[0]);
                    /* pthread_join(pid[0], NULL); */
                    /* pid[0] = 0; */
                } else if (pid[1] == 0) {
                    thread_arg arg = {
                        .pstart = p + pos,
                        .text_len = text_len - pos,
                        .pattern = pattern,
                        .pat_len = pat_len,
                        .num = 1,
                        .result = -1,
                    };
                    pthread_create(&pid[1], NULL, fn, &arg);
                    dprintf(INFO, "thread %u starts on 1\n", pid[1]);
                } else {
                    continue;
                }
                /* if (arg.result < 0) { */
                /*     break; */
                /* } else { */
                /*     pos += arg.result + pat_len; */
                /*     printf("pos: %d -- %.*s\n", pos, 20, p + pos - pat_len); */
                /* } */
#else
                int result = fn(p + pos, text_len - pos, pattern, pat_len);
                if (result < 0) {
                    break;
                } else {
                    pos += result + pat_len;
                    printf("pos: %d -- %.*s\n", pos, 20, p + pos - pat_len);
                }
#endif
            }

            munmap(p, st.st_size);
            close(fd);
        }
    }

    if (closedir(d) < 0) {
        dprintf(ERROR, "closedir failed");
        exit(-1);
    }
}

int main(int argc, char *argv[])
{
#ifdef TEST_KMP
    if (argc < 2)
        return 1;

    int fd = open(argv[1], O_RDONLY);
    char *p = mmap(0, 4096, PROT_READ, MAP_SHARED, fd, 0);
    /* printf("%d\n", p); */
    char str[16];
    snprintf(str, 10, "%s\n", p);
    printf("%s", str);
    char *pattern = "ab";

    int pos = kmp(p, pattern);
    printf("%d\n", pos);
    snprintf(str, 10, "%s\n", p+pos+1);
    printf("%s", str);
    munmap(p, strlen(p));
    close(fd);
    return 0;
#else
    parse_options(argc, argv);

#ifndef TEST_DIR
    /* default to search in the current directory */
    if (!opt.input_file) {
        dprintf(ERROR, "err_exit");
        exit(-1);
    }

    /* open file for searching
     * use mmap in the future */
    /* TODO: tilde-expansion ? */
    int fd;
    if ((fd = open(opt.input_file, O_RDONLY)) == -1) {
        dprintf(ERROR, "fail to open file for read");
        exit(-1);
    }

    /* map the file to a space of 4K bytes */
    char *p;
    if ((p = mmap(0, 4096 * 1024, PROT_READ, MAP_SHARED, fd, 0)) == MAP_FAILED) {
        dprintf(EROR, "fail to map the file");
        exit(-1);
    }

    char str[16];                         /* to hold the strings about the loc of pos */
    char *pattern = opt.search_pattern;

    int text_len, pat_len;

    pat_len = strlen(pattern);
    if (p[4095])
        text_len = 4096 * 1024;
    else
        text_len = strlen(p);

    printf("%d %d\n", text_len, pat_len);
    /* start searching using KMP algorithm */
    int pos = kmp_match(p, text_len, pattern, pat_len);

    printf("%d\n", pos);
    if (pos > 0) {
        /* snprintf(str, 15, "%s\n", p+pos+1); */
        /* /\* printf("%sssss\n", str); *\/ */
        /* fprintf(stdout, "%ssssssssssss\n", str); */
        /* fflush(NULL); */
    }
    /* printf("ssssssssssssss\n"); */

    munmap(p, 4096 * 1024);
    close(fd);
#else
    if (!opt.input_dir) {
        dprintf(ERROR, "missing input_dir");
        exit(-1);
    }
    printf("search in %s\n", opt.input_dir);
    myftw(opt.input_dir, kmp_match);
#endif
    return 0;
#endif
}
