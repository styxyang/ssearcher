/*
 * buffer.c: Manage strings and buffers
 * Copyright (c) 2013, Yang Hong
 * All rights reserved.
 */

#include "debug.h"
#include "config.h"
#include "buffer.h"
#include "util.h"

#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/mman.h>

#ifdef DEBUG
#include <assert.h>
#endif

#define DEFAULT_SIZE 4096

__thread char  *buf = NULL;
__thread size_t cap = 0;
__thread size_t off = 0;

extern __thread long tid;

#ifdef DEBUG
static long grow_cnt = 0;
#endif

#ifdef ROPE
/* set_rope set the string content of a leaf rope */
static inline void rope_set(struct rope *r, const char *str, uint8_t tag)
{
    r->pstr = strdup(str);
    r->tag = tag;
}

static inline void rope_setn(struct rope *r, const char *str, uint32_t len, uint8_t tag)
{
    r->pstr = strndup(str, len);
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

static inline bool rope_isleaf(struct rope *r)
{
    switch (r->tag) {
        case TAG_DEFAULT:
            return false;
        default:
            return true;
    }
}

/* static void rope_color(struct rope *r, uint32_t offset) */
/* { */
/* } */

void buf_init(buffer *buf)
{
    INIT_LIST_HEAD(&buf->ropelist);
}

void buf_destroy(buffer *buf)
{
    struct rope *p, *prefetch;
    for (p = list_entry(buf->ropelist.next, struct rope, lh),
                 prefetch = list_entry(p->lh.next, struct rope, lh);
         &p->lh != &buf->ropelist;
         p = prefetch, prefetch = list_entry(p->lh.next, struct rope, lh)) {
        free(p->pstr);
        free(p);
    }
}

knot buf_write(buffer *buf, const char *content, uint32_t len, uint8_t tag)
{
    struct rope *r = (struct rope *)malloc(sizeof(*r));
    rope_setn(r, content, len, tag);
    list_add_tail(&r->lh, &buf->ropelist);
    return r;
}

knot buf_writeln(buffer *buf, const char *content, uint8_t tag)
{
    struct rope *r = (struct rope *)malloc(sizeof(*r));
    /* rope_setn(r, content, len, tag); */
    /* list_add_tail(&r->lh, &buf->ropelist); */
    return r;
}

void buf_dump(buffer *buf)
{
    struct rope *p;
    list_for_each_entry(p, &buf->ropelist, lh) {
        if (rope_isleaf(p)) {
            printf("%s\n", p->pstr);
        }
    }
}

/*********************************************************
 *
 * extended functions
 *
 ********************************************************/

/**
 * color a segment of existing rope with offset
 * from the beginning of the line
 */
int buf_color(knot which, uint32_t off, uint32_t len)
{
    struct rope *r = which;
    while (r) {
        if (rope_isleaf(r)) {
            return -1;  /* reach leaf rope */
        }
        if (off < r->offset) {
            r = r->left;
            continue;
        }
        if (off >= r->offset) {
            r = r->right;
            continue;
        }
    }
    return -1;  /* invalid rope */
}

void buf_write_offset(buffer *buf, const char *content, uint32_t len, uint32_t off)
{
    /* FIXME select the tag */
    struct rope *r = buf_write(buf, content, len, TAG_CONTEXT);
    r->offset = off;
}
#endif  /* ROPE */

static void buffer_grow(size_t len)
{
    size_t newcap = cap;
#ifdef DEBUG
    grow_cnt++;
    dprintf(WARN, "T%d C%d old buffer=%p cap=%u(%x) off=%u(%x)",tid, grow_cnt, buf, cap, cap, off, off);
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
    buf = realloc(buf, sizeof(char) * newcap);
    memset(buf + off, 0, newcap - off);
    cap = newcap;
#ifdef DEBUG
    dprintf(WARN, "T%d C%d new buffer=%p cap=%u(%x) off=%u(%x)",tid, grow_cnt, buf, cap, cap, off, off);
#endif
}

void init_buffer()
{
    if (buf != NULL) {
#ifdef DEBUG
        dprintf(WARN, "T%d before realloc buffer=%p cap=%u(%x) off=%u(%x)", tid, buf, cap, cap, off, off);
#endif
        buf = realloc(buf, DEFAULT_SIZE * sizeof(char));

#ifdef DEBUG
        dprintf(WARN, "T%d after realloc buffer=%p cap=%u(%x) off=%u(%x)", tid, buf, DEFAULT_SIZE, DEFAULT_SIZE, 0, 0);
#endif
    } else {
        buf = malloc(DEFAULT_SIZE * sizeof(char));
    }
    memset(buf, 0, DEFAULT_SIZE);
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
    memcpy(buf + off, content, len);
    off += len;
#ifdef DEBUG
    dprintf(INFO, "T%d write to result buffer %p:%x:%x", tid, buf, cap, off);
    if (strlen(buf) != off) {
        dprintf(ERROR, "%d:%d", strlen(buf), off);
        assert(strlen(buf) == off);
    }
#endif
    return len;
}

char *read_buffer()
{
    return buf;
}

void destroy_buffer()
{
    if (buf)
        free(buf);
    buf = NULL;
    cap = 0;
    off = 0;
}

void reset_buffer()
{
    dprintf(WARN, "T%d buffer=%p, cap=%u, off=%u", tid, buf, cap, off);
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
    dprintf(INFO, "T%d write to result buffer %p:%x:%x", tid, buf, cap, off);
    if (off + len > cap) {
        dprintf(WARN, "insufficient buffer");
        buffer_grow(len);
    }
    int i;
    for (i = 0; i < len && content[i] != '\n'; i++)
        ;
    memcpy(buf + off, content, i);
    off += i;
#ifdef DEBUG
    if (strlen(buf) != off) {
        dprintf(ERROR, "%d:%d", strlen(buf), off);
        assert(strlen(buf) == off);
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
    char *bufferend = buf + off;

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
    dprintf(INFO, "T%d write to result buffer %p:%x:%x", tid, buf, cap, off);
    if (strlen(buf) != off) {
        dprintf(ERROR, "%d:%d", strlen(buf), off);
        assert(strlen(buf) == off);
    }
#endif
    return reststr - bufferend + i;
}

size_t amendline_color_buffer(size_t lastlen, size_t off1, size_t colorlen, int cnt)
{
    dprintf(INFO, "T%d write to result buffer %p:%u(%x):%u(%x)-%d", tid, buf, cap, cap, off, off, strlen(buf));
    /* dprintf(ERROR, ">>> %s", buffer); */
    if (off + sizeof(HILI_COLOR) + 3 > cap) {
        dprintf(WARN, "insufficient buffer");
        buffer_grow(sizeof(HILI_COLOR) + 3);
    }

    char *amendstart = buf + off - lastlen
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
    if (strlen(buf) != off) {
        dprintf(ERROR, "T%d %d:%d", tid, strlen(buf), off);
        dprintf(ERROR, "--> %s\n", buf);
        assert(strlen(buf) == off);
    }
#endif
    return sizeof(HILI_COLOR) + 3;
}
