#ifndef SS_BUFFER_H
#define SS_BUFFER_H

#include "config.h"
#include "util.h"
#include <sys/types.h>

#ifdef ROPE
struct rope {
    struct list_head lh;
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
#define TAG_MISCTXT 4  /* miscellaneous text, such as line number */

#endif  /* ROPE */

/* result buffer of match records in one file
 * it should be embedded in file structures b/c it
 * actually belongs to one file
 */
typedef struct {
    struct list_head ropelist;
} buffer;

void buf_init(buffer *buf);
void buf_destroy(buffer *buf);
size_t buf_write(buffer *buf, const char *content, size_t len);
void buf_dump(buffer *buf);

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
