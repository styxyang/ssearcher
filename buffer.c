/*
 * buffer.c: Manage strings and buffers
 * Copyright (c) 2013, Yang Hong
 * All rights reserved.
 */

#include "debug.h"
#include "config.h"
#include "buffer.h"

#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/mman.h>

#ifdef DEBUG
#include <assert.h>
#endif

#define DEFAULT_SIZE 4096

__thread char  *buffer = NULL;
__thread size_t cap = 0;
__thread size_t off = 0;

extern __thread long tid;

#ifdef DEBUG
static long grow_cnt = 0;
#endif

/* set_rope set the string content of a leaf rope */
static inline void rope_set(struct rope *r, const char *str, uint8_t tag)
{
    r->pstr = strdup(str);
    r->tag = tag;
}

/* tear_rope destroy a rope */
static inline void rope_tear(struct rope *r)
{
    if (r->isleaf)
    free(r->pstr);
}

/* join_rope join two ropes as a new rope */
static inline void rope_join(struct rope *r, uint32_t offset, struct rope *r1, struct rope *r2)
{
    r->left = r1;
    r->right = r2;
    r->isleaf = 0;
    r->tag = TAG_DEFAULT;
}

static void buffer_grow(size_t len)
{
    size_t newcap = cap;
#ifdef DEBUG
    grow_cnt++;
    dprintf(WARN, "T%d C%d old buffer=%p cap=%u(%x) off=%u(%x)",tid, grow_cnt, buffer, cap, cap, off, off);
#endif

    /* every time double the space */
    while (newcap < len + off) {
        newcap += newcap;
    }
    /* char *tmp = malloc(sizeof(char) * newcap); */
    /* memcpy(tmp, buffer, off); */
    /* free(buffer); */
    /* buffer = tmp; */
    /* cap = newcap; */
    buffer = realloc(buffer, sizeof(char) * newcap);
    memset(buffer + off, 0, newcap - off);
    cap = newcap;
#ifdef DEBUG
    dprintf(WARN, "T%d C%d new buffer=%p cap=%u(%x) off=%u(%x)",tid, grow_cnt, buffer, cap, cap, off, off);
#endif
}

void init_buffer()
{
    if (buffer != NULL) {
#ifdef DEBUG
        dprintf(WARN, "T%d before realloc buffer=%p cap=%u(%x) off=%u(%x)", tid, buffer, cap, cap, off, off);
#endif
        buffer = realloc(buffer, DEFAULT_SIZE * sizeof(char));

#ifdef DEBUG
        dprintf(WARN, "T%d after realloc buffer=%p cap=%u(%x) off=%u(%x)", tid, buffer, DEFAULT_SIZE, DEFAULT_SIZE, 0, 0);
#endif
    } else {
        buffer = malloc(DEFAULT_SIZE * sizeof(char));
    }
    memset(buffer, 0, DEFAULT_SIZE);
    cap = DEFAULT_SIZE;
    off = 0;

#ifdef DEBUG
    grow_cnt = 0;
#endif
}

size_t write_buffer(const char *content, size_t len)
{
    /* may grow several times util enough */
    if (off + len > cap) {
        dprintf(WARN, "insufficient buffer");
        buffer_grow(len);
    }
    memcpy(buffer + off, content, len);
    off += len;
#ifdef DEBUG
    dprintf(INFO, "T%d write to result buffer %p:%x:%x", tid, buffer, cap, off);
    if (strlen(buffer) != off) {
        dprintf(ERROR, "%d:%d", strlen(buffer), off);
        assert(strlen(buffer) == off);
    }
#endif
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
    dprintf(WARN, "T%d buffer=%p, cap=%u, off=%u", tid, buffer, cap, off);
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
    dprintf(INFO, "T%d write to result buffer %p:%x:%x", tid, buffer, cap, off);
    if (off + len > cap) {
        dprintf(WARN, "insufficient buffer");
        buffer_grow(len);
    }
    int i;
    for (i = 0; i < len && content[i] != '\n'; i++)
        ;
    memcpy(buffer + off, content, i);
    off += i;
#ifdef DEBUG
    if (strlen(buffer) != off) {
        dprintf(ERROR, "%d:%d", strlen(buffer), off);
        assert(strlen(buffer) == off);
    }
#endif
    return i;
}

/**
 * write to buffer a line with specified characters hightlighted
 * from `off1' with `length' and total length within `len'
 *
 * @param content  string that is going to be appended to buffer
 * @param len      max length to copy
 * @param off1     offset of the patched string in content string
 * @param colorlen length of highlight
 */
size_t writeline_color_buffer(const char *content, size_t len,
                              size_t off1, size_t colorlen)
{
    if (off + len + sizeof(HILI_COLOR) + 3 > cap) {
        dprintf(WARN, "insufficient buffer %x", len);
        buffer_grow(len + sizeof(HILI_COLOR) + 3);
    }

    /* where in the buffer to start copying content to */
    char *bufferend = buffer + off;

    /* where to put the first part of highlight code */
    char *hilitoken = bufferend + off1;

    /* `sizeof' a string includes the null terminator */
    char *patstart  = hilitoken + sizeof(HILI_COLOR) - 1;

    /* where to put the last part of highlight code */
    char *hiliend   = patstart + colorlen;

    /* where to put the content left after the highlighted content */
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
#ifdef DEBUG
    dprintf(INFO, "T%d write to result buffer %p:%x:%x", tid, buffer, cap, off);
    if (strlen(buffer) != off) {
        dprintf(ERROR, "%d:%d", strlen(buffer), off);
        assert(strlen(buffer) == off);
    }
#endif
    return reststr - bufferend + i;
}

size_t amendline_color_buffer(size_t lastlen, size_t off1, size_t colorlen, int cnt)
{
    dprintf(INFO, "T%d write to result buffer %p:%u(%x):%u(%x)-%d", tid, buffer, cap, cap, off, off, strlen(buffer));
    /* dprintf(ERROR, ">>> %s", buffer); */
    if (off + sizeof(HILI_COLOR) + 3 > cap) {
        dprintf(WARN, "insufficient buffer");
        buffer_grow(sizeof(HILI_COLOR) + 3);
    }

    char *amendstart = buffer + off - lastlen
            + off1 + cnt * (sizeof(HILI_COLOR) + 3);
    char *amendend   = amendstart + colorlen;
    char *restline   = amendend + sizeof(HILI_COLOR) + 3;

    /* copy rest of the line excluding the pattern part */
    strcpy(restline, amendend);
    /* move the pattern to make space for HILI_COLOR */
    memcpy(amendstart + sizeof(HILI_COLOR) - 1, amendstart, colorlen);
    /* fill in the HILI_COLOR */
    memcpy(amendstart,
           HILI_COLOR, sizeof(HILI_COLOR) - 1);
    /* fill in the reset part */
    memcpy(amendstart + sizeof(HILI_COLOR) - 1 + colorlen
           , "\e[0m", 4);
    off += sizeof(HILI_COLOR) + 3;
#ifdef DEBUG
    if (strlen(buffer) != off) {
        dprintf(ERROR, "T%d %d:%d", tid, strlen(buffer), off);
        dprintf(ERROR, "--> %s\n", buffer);
        assert(strlen(buffer) == off);
    }
#endif
    return sizeof(HILI_COLOR) + 3;
}
