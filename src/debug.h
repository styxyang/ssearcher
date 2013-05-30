#ifndef SS_DEBUG_H
#define SS_DEBUG_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
/* #define SS_DEBUG */

#define MAXLINE 128

#define DEBUG_HERE                                      \
  do {                                                  \
    fprintf(stderr, "(D) %s:%d\n", __FILE__, __LINE__); \
  } while (0);

static void err_exit(const char *fmt, ...) {
  va_list ap;
  char buf[MAXLINE];                    /* TODO: Buffer overflow ? find better solution */

  va_start(ap, fmt);

  vsnprintf(buf, MAXLINE, fmt, ap);
  if (sizeof(buf) - strlen(buf) > 1) {
    strncat(buf, "\n", 1);
    fflush(stdout);                     /* in case stdout and stderr are the same */
    fputs(buf, stderr);                 /* TODO: any security issues ? */
    fflush(NULL);                       /* flushes all stdio output streams */
    va_end(ap);
    exit(1);
  } else {
    fprintf(stderr, "debug routine fails: %s:%d", __FILE__, __LINE__);
    exit(-1);
  }
}

#endif  /* SS_DEBUG_H */
