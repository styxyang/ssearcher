#include "ss_debug.h"
#include "ss_config.h"
#include "ss_buffer.h"

#include <string.h>
#include <stdarg.h>
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
    return len;
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

size_t writef_buffer(const char *format, ...)
{
    /* may grow several times util enough */
    va_list args;
    char    str[512];
    int     n;
    memset(str, 0, sizeof(str));

    va_start(args, format);

    /* the correct usage of va is to use `va_arg' and specify the type */
    n = sprintf(str, format, va_arg(args, uint32_t));
    write_buffer(str, n);
    va_end(args);
    return n;
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
    return i;
}

/* write to buffer a line with specified characters hightlighted
 * from `off1' with `length' and total length within `len'
 *
 * \param content  string that is going to be appended to buffer
 * \param len      max length to copy
 * \param off1     offset of the patched string in content string
 * \param colorlen length of highlight */
size_t writeline_color_buffer(const char *content, size_t len,
                              size_t off1, size_t colorlen)
{
    if (off + len > cap) {
        buffer_grow(len);
    }

    char *bufferend = buffer + off;
    char *hilitoken = bufferend + off1;

    /* `sizeof' a string includes the null terminator */
    char *patstart  = hilitoken + sizeof(HILI_COLOR) - 1;
    char *hiliend   = patstart + colorlen;
    char *reststr   = hiliend + 4;  /* "\e[0m" */
    const char *restctnt  = content + off1 + colorlen;

    memcpy(bufferend, content, off1);

    memcpy(hilitoken, HILI_COLOR, sizeof(HILI_COLOR) - 1);

    memcpy(patstart, content + off1, colorlen);

    memcpy(hiliend, "\e[0m", 4);

    int i;
    for (i = 0; i < len - off1 - colorlen && restctnt[i] != '\n' && restctnt[i] != '\0'; i++)
        ;
    memcpy(reststr, restctnt, i);
    off += (uint32_t)(reststr - bufferend) + i;

    return reststr - bufferend + i;
}


