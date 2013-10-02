#ifndef SS_BUFFER_H
#define SS_BUFFER_H

#include <sys/types.h>

struct rope {
    struct rope *left;
    struct rope *right;

    /* GCC 4.7 starts supporting anonymous struct/union
     * within structs/unions */
    union {
        char *pstr;
        char  isleaf;
    };

    /* location of the join point */
    uint32_t offset;

    /* determine which kind of rope it is */
    uint8_t tag;
} __attribute__ ((aligned(64)));

#define TAG_DEFAULT 0  /* default, for inner nodes */
#define TAG_CONTEXT 1  /* context, for non-keyword text */
#define TAG_KEYWORD 2  /* keyword, the search target */
#define TAG_ANSIESC 3  /* ansi color escape, for coloring the keyword */

struct filerecord {
    char *filename;
    uint32_t matchcnt;
    struct rope *matchlst;
} __attribute__ ((aligned(64)));

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
