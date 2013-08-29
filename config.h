#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <sys/types.h>

/* #define USE_PTHREAD */
#ifndef uint64_t
typedef unsigned long long uint64_t;
#endif

#ifndef uint32_t
typedef unsigned int uint32_t;
#endif

#ifndef int32_t
typedef int int32_t;
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

#define likely(x)   __builtin_expect((x), true)
#define unlikely(x) __builtin_expect((x), false)

#define NCPU 2
#define NTHR 3

typedef struct th_arg {
    char *pstart;
    int   text_len;
    char *pattern;
    int   pat_len;
    int   num;
    int   result;
} thread_arg;

#endif  /* _CONFIG_H_ */
