#ifndef SS_FILE_H
#define SS_FILE_H

#include "ss_config.h"

void *map_file(int fd);
bool unmap_file(void *addr);
bool inbound(uint32_t pos);

#endif  /* SS_FILE_H */
