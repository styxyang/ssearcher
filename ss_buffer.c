#include "ss_debug.h"
#include "ss_config.h"
#include "ss_buffer.h"

#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>

#define DEFAULT_SIZE 4096

__thread char  *buffer = NULL;
__thread size_t cap = 0;
__thread size_t off = 0;

static inline void buffer_grow(size_t len)
{
    size_t newcap = cap;
    /* every time double the space */
    while (newcap - off < len) {
        newcap += newcap;
    }
    char *tmp = malloc(sizeof(char) * newcap);
    memcpy(tmp, buffer, off);
    free(buffer);
    buffer = tmp;
    cap = newcap;
}

void init_buffer()
{
    if (buffer)
        free(buffer);
    buffer = malloc(DEFAULT_SIZE * sizeof(char));
    memset(buffer, 0, DEFAULT_SIZE);
    cap = DEFAULT_SIZE;
    off = 0;
}

size_t write_buffer(const char *content, size_t len)
{
    /* may grow several times util enough */
    if (off + len > cap) {
        buffer_grow(len);
    }
    memcpy(buffer + off, content, len);
    off += len;
}

char *read_buffer()
{
    return buffer;
}

void destroy_buffer()
{
    if (buffer)
        free(buffer);
    buffer = NULL;
    cap = 0;
    off = 0;
}

void reset_buffer()
{
    init_buffer();
}

size_t writeline_buffer(const char *content, size_t len)
{
    if (off + len > cap) {
        buffer_grow(len);
    }
    int i;
    for (i = 0; i < len && content[i] != '\n'; i++)
        ;
    memcpy(buffer + off, content, i);
    off += i;
}

/* write to buffer a line with specified characters hightlighted
 * from `off1' with `length' and total length within `len' */
size_t writeline_color_buffer(const char *content, size_t len, size_t off1, size_t colorlen)
{
    if (off + len > cap) {
        buffer_grow(len);
    }

    memcpy(buffer + off, content, off1);
    off += off1;

    memcpy(buffer + off, HILI_COLOR, strlen(HILI_COLOR));
    off += strlen(HILI_COLOR);

    memcpy(buffer + off, content + off1, colorlen);
    off += colorlen;

    memcpy(buffer + off, "\e[0m", 4);
    off += 4;

    int i;
    for (i = 0; i < len && content[i + off1  + colorlen] != '\n'; i++)
        ;
    memcpy(buffer + off, content + off1 + colorlen, i);
    off += i;
}

