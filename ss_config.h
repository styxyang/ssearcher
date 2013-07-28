#ifndef _CONFIG_H_
#define _CONFIG_H_

/* #define USE_PTHREAD */
#ifndef uint64_t
typedef unsigned long long uint64_t;
#endif

#ifndef uint32_t
typedef unsigned int uint32_t;
#endif

#ifndef uint8_t
typedef unsigned char     uint8_t;
#endif
#ifndef __cplusplus
typedef unsigned char     bool;
#  ifndef true
#    define true (1)
#  endif
#  ifndef false
#    define false (0)
#  endif
#endif

#define NCPU 1
#define NTHR 2

typedef struct th_arg {
    char *pstart;
    int   text_len;
    char *pattern;
    int   pat_len;
    int   num;
    int   result;
} thread_arg;

#define TEST_PROCON

#endif /* _CONFIG_H_ */
