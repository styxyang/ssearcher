#ifndef SS_DEBUG_H
#define SS_DEBUG_H

#define SS_DEBUG 0

#define DEBUG_HERE                                      \
  do {                                                  \
    fprintf(stderr, "(D) %s:%d\n", __FILE__, __LINE__); \
  } while (0);

#endif  /* SS_DEBUG_H */
