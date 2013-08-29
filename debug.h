#ifndef SS_DEBUG_H
#define SS_DEBUG_H

#include <stdio.h>
#include <stdlib.h>

/* #define DEBUG */

#define ERROR   0
#define WARN    1
#define INFO    2

/* colors for different debug level
 * HILI uses xterm-256color and may not display
 * correctly on legacy terminals */
#define ERROR_COLOR  "\e[1;31m"
#define WARN_COLOR   "\e[1;33m"
#define INFO_COLOR   "\e[1;34m"
#define FNAME_COLOR  "\e[1;34m"
#define LINUM_COLOR  "\e[1;35m"
#define HILI_COLOR   "\e[48;5;220m\e[30;22m"

#ifdef DEBUG
#define dprintf(level, fmt, ...)                                        \
    do {                                                                \
        fprintf(stderr, level##_COLOR "[%s:%d] %s: " fmt "\e[0m\n",     \
                __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__);       \
    } while (0)
#else
#define dprintf(level, fmt, ...)                \
    do {                                        \
    } while (0)
#endif

#define die(fmt, ...)                                                   \
    do {                                                                \
        fprintf(stderr, ERROR_COLOR "[%s:%d] %s: " fmt "\e[0m\n",       \
                __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__);       \
        exit(1);                                                        \
    } while (0)

#endif /* SS_DEBUG_H */
