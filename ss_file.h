#ifndef SS_FILE_H
#define SS_FILE_H

#include "ss_config.h"

#define MAXPATH 255

struct fileinfo {
    int fd;
    char filename[MAXPATH];
};

void *map_file(int fd);
bool unmap_file(void *addr);
bool inbound(uint32_t pos);
uint32_t map_len(int fd);

#endif  /* SS_FILE_H */
