#ifndef SS_DEBUG_H
#define SS_DEBUG_H

#include <stdio.h>

#define DEBUG

#define ERROR   0
#define WARN    1
#define INFO    2

/* colors for different debug level */
#define ERROR_COLOR  "\e[1;31m"
#define WARN_COLOR   "\e[1;33m"
#define INFO_COLOR   "\e[1;34m"

#ifdef DEBUG
#define dprintf(level, fmt, ...)                                        \
    do {                                                                \
        fprintf(stderr, level##_COLOR "[%s:%d] %s: " fmt "\e[0m",           \
                __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__);       \
    } while (0)
#else
#define dprintf(level, fmt, ...)                \
    do {                                        \
    } while (0)
#endif

#endif /* SS_DEBUG_H */
