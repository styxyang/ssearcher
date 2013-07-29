#ifndef SS_BUFFER_H
#define SS_BUFFER_H

#include <sys/types.h>

void init_buffer();
size_t write_buffer(const char *content, size_t clen);
char *read_buffer();
void reset_buffer();
void destroy_buffer();

#endif  /* SS_BUFFER_H */
