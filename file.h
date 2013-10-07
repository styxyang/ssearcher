#ifndef SS_FILE_H
#define SS_FILE_H

#include "config.h"
#include "buffer.h"

#define MAXPATH 255

typedef struct {
    int    fd;
    char  *filename;  /* may resort to `readlink' */
    char  *pmap;      /* pointer to the mapped file in memory */
    size_t size;
    rbuf   rb;        /* result buffer */
} fileinfo;

void *map_file(fileinfo *fi);
bool unmap_file(fileinfo *fi);
bool inbound(uint32_t pos);

#endif  /* SS_FILE_H */
