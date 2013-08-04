#include "ss_config.h"
#include "ss_options.h"
#include "ss_match.h"
#include "ss_debug.h"
#include "ss_thread.h"

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

/* #define TEST_KMP */
/* #define TEST_DIR */

pthread_t pid[NTHR];
/* int finish[2] = { 0, 0 }; */
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

int test_kmp(int argc, char *argv[])
{
    if (argc < 2)
        return 1;

    int fd = open(argv[1], O_RDONLY);
    char *p = mmap(0, 4096, PROT_READ, MAP_SHARED, fd, 0);
    /* printf("%d\n", p); */
    char str[16];
    snprintf(str, 10, "%s\n", p);
    printf("%s", str);
    char *pattern = "ab";

    int pos = kmp_match(p, strlen(p), pattern, 2);
    printf("%d\n", pos);
    snprintf(str, 10, "%s\n", p+pos+1);
    printf("%s", str);
    munmap(p, strlen(p));
    close(fd);
    return 0;
}

void test_file()
{
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
        dprintf(ERROR, "fail to map the file");
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
}

int pipefd[2];
/* char *ss_result[NCPU]; */
int ss_result[NCPU][2];

void ss_init()
{
    int i;

    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    /* FIXME: use ssmalloc? what if overflowed? */
    /* for (i = 0; i < NCPU; i++) { */
    /*     ss_result[i] = (char *)malloc(sizeof(char) * 4096); */
    /* } */

    for (i = 0; i < NCPU; i++) {
        if (pipe(ss_result[i]) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }
}

void ss_exit()
{
    int i;

    /* for (i = 0; i < NCPU; i++) { */
    /*     free(ss_result[i]); */
    /* } */
    close(pipefd[0]);
}

int ss_check_buffer()
{
    int i;

    struct pollfd fdset[NCPU];
    memset(&fdset, 0, sizeof(fdset));

    for (i = 0; i < NCPU; ++i) {
        fdset[i].fd = ss_result[i][0];
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
    pthread_create(&pid[0], NULL, ss_dispatcher_thread, NULL);
    pthread_create(&pid[1], NULL, ss_worker_thread, (void *)0);
    /* pthread_create(&pid[2], NULL, ss_worker_thread, NULL); */
    ss_check_buffer();
    pthread_join(pid[0], NULL);
    pthread_join(pid[1], NULL);
    pthread_mutex_destroy(&outmtx);
}


int main(int argc, char *argv[])
{
#if defined(TEST_KMP)
    return test_kmp(argc, argv);
#elif defined(TEST_DIR)
    parse_options(argc, argv);
    if (!opt.input_dir) {
        dprintf(ERROR, "missing input_dir");
        exit(-1);
    }
    printf("search in %s\n", opt.input_dir);
    myftw(opt.input_dir, kmp_match);
    return 0;
#elif defined(TEST_FILE)
    parse_options(argc, argv);
    test_file();
    return 0;
#elif defined(TEST_PROCON)
    parse_options(argc, argv);
    ss_init();
    test_procon();
    ss_exit();
    return 0;
#endif
}
