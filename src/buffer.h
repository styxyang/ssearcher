#ifndef SS_BUFFER_H
#define SS_BUFFER_H

#include "config.h"
#include "util.h"
#include <sys/types.h>

typedef struct {
    struct list_head record_list;
} buffer;

/* Old interface */
void init_buffer();
size_t write_buffer(const char *content, size_t len);
size_t writef_buffer(const char *format, ...);
size_t writeline_buffer(const char *content, size_t len);
size_t writeline_color_buffer(const char *content, size_t len, size_t off1, size_t colorlen);
size_t amendline_color_buffer(size_t lastlen, size_t off1, size_t colorlen, int cnt);
char *read_buffer();
void reset_buffer();
void destroy_buffer();

#endif  /* SS_BUFFER_H */
