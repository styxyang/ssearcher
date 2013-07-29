#ifndef SS_FILE_H
#define SS_FILE_H

#include "ss_config.h"

void *map_file(int fd);
bool unmap_file(void *addr);

#endif  /* SS_FILE_H */
