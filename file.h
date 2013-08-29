#ifndef SS_FILE_H
#define SS_FILE_H

#include "config.h"

#define MAXPATH 255

typedef struct {
    int    fd;
    char   filename[MAXPATH];  /* may resort to `readlink' */
    char  *pmap;
    size_t size;
} fileinfo;

void *map_file(fileinfo *fi);
bool unmap_file(fileinfo *fi);
bool inbound(uint32_t pos);

#endif  /* SS_FILE_H */
