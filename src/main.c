#include "options.h"
#include "debug.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

#include <dirent.h>
#include <sys/stat.h>
#include <limits.h>

#include <pthread.h>

/* #define TEST_KMP */
#define TEST_DIR

#define USE_PTHREAD

pthread_t pid[2] = { 0, 0 };

typedef struct th_arg {
    char *pstart;
    int   text_len;
    char *pattern;
    int   pat_len;
    int   result;
} thread_arg;

void init_kmp_table(int *kmp_table, char *pattern, unsigned int pat_len) {
    /* if (pat_len < 1 */
    kmp_table[0] = 0;
    int i;

    for (i = 1; i < pat_len; i++) {
        if (pattern[kmp_table[i - 1]] == pattern[i]) {
            kmp_table[i] = kmp_table[i - 1] + 1;
        } else {
            kmp_table[i] = 0;
        }
    }
}

#ifdef USE_PTHREAD
void kmp(void *)
#else
int kmp(char *text, int text_len, char *pattern, int pat_len)
#endif
{
    int max_match = 0;
    int pat_idx = 0;

    int text_pos = 0;
    int found = 0;

    int *kmp_table = (int *)malloc(sizeof(int) * pat_len);
    init_kmp_table(kmp_table, pattern, pat_len);

    while (text_pos < text_len) {
        /* printf("%d %d %c %c\n", text_pos, pat_idx, text[text_pos], pattern[pat_idx]); */
        if (text[text_pos] == pattern[pat_idx]) {
            if (pat_idx == pat_len - 1)
                goto found;
            else {
                pat_idx++;
                text_pos++;
            }
        } else {
            /* fix this ? */
            if (pat_idx == 0) {
                text_pos++;
                continue;
            }
            pat_idx = kmp_table[pat_idx - 1];
            if (text[text_pos] == pattern[pat_idx]) {
                if (pat_idx == pat_len - 1) {
                    goto found;
                } else {
                    pat_idx++;
                    text_pos++;
                }
            } else if (pat_idx != 0) {
                pat_idx = 0;
            } else {
                text_pos++;
            }
        }
    }
    free(kmp_table);
    return -1;
found:
    /* would it be unnecessary to freeit?
     * since it will be only executed quickly and then terminated */
    free(kmp_table);
    return text_pos - pat_len + 1;
}

void myftw(const char *dirname, int (*fn)(char *, int, char *, int)) {
    struct stat    statbuf;
    DIR           *d;
    struct dirent *entry;

    if (lstat(dirname, &statbuf) < 0)    /* stat error */
        err_exit("myftw: lstat error: %s:%d", __FILE__, __LINE__);

    if (S_ISDIR(statbuf.st_mode) == 0)    /* not a directory */
        err_exit("myftw: target not a directory");

    if ((d = opendir(dirname)) == NULL) {
        err_exit("myftw: opendir failed");
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
                err_exit("ss: fail to open file for read: %s", fullpath);
            }

            /* map the file to a space of 4M bytes */
            char *p;
            if ((p = mmap(0, st.st_size, PROT_READ, MAP_SHARED, fd, 0)) == MAP_FAILED) {
                err_exit("ss: fail to map the file");
            }

            char str[16];                         /* to hold the strings about the loc of pos */
            char *pattern = opt.search_pattern;

            int text_len, pat_len;

            pat_len = strlen(pattern);
            /* if (p[4095]) */
            /*     text_len = 4096; */
            /* else */
            /*     text_len = strlen(p); */
            text_len = st.st_size;

            printf("%d %d\n", text_len, pat_len);

            int pos = 0;
            while (pos < st.st_size) {
#ifdef USE_PTHREAD
                int result;
                __thread thread_arg arg = {
                    .pstart = p + pos,
                    .text_len = text_len - pos,
                    .pattern = pattern,
                    .pat_len = pat_len,
                    .result = -1;
                };
                if (pid[0] == 0) {
                    pthread_create(&pid[0], NULL, fn, thread_arg);
                    pthread_join(pid[0], NULL);
                } else if (pid[1] == 0) {
                    pthread_create(&pid[1], NULL, fn, thread_arg);
                    pthread_join(pid[1], NULL);
                } else {
                    continue;
                }
#else
                int result = fn(p + pos, text_len - pos, pattern, pat_len);
#endif
                if (result < 0) {
                    break;
                } else {
                    pos += result + pat_len;
                    printf("pos: %d -- %.*s\n", pos, 20, p + pos - pat_len);
                }
            }

            munmap(p, st.st_size);
            close(fd);
        }
    }

    if (closedir(d) < 0) {
        err_exit("myftw: closedir failed");
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
        err_exit("err_exit");
    }

    /* open file for searching
     * use mmap in the future */
    /* TODO: tilde-expansion ? */
    int fd;
    if ((fd = open(opt.input_file, O_RDONLY)) == -1) {
        err_exit("ss: fail to open file for read");
    }

    /* map the file to a space of 4K bytes */
    char *p;
    if ((p = mmap(0, 4096 * 1024, PROT_READ, MAP_SHARED, fd, 0)) == MAP_FAILED) {
        err_exit("ss: fail to map the file");
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
    int pos = kmp(p, text_len, pattern, pat_len);

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
        err_exit("ss: missing input_dir");
    }
    printf("search in %s\n", opt.input_dir);
    myftw(opt.input_dir, kmp);
#endif
    return 0;
#endif
}
