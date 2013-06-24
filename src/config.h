#ifndef _CONFIG_H_
#define _CONFIG_H_

/* #define USE_PTHREAD */
typedef unsigned long int uint64_t;
typedef unsigned char     uint8_t;

typedef struct th_arg {
    char *pstart;
    int   text_len;
    char *pattern;
    int   pat_len;
    int   num;
    int   result;
} thread_arg;

#endif /* _CONFIG_H_ */
