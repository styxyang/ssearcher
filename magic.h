#ifndef SS_MAGIC_H
#define SS_MAGIC_H

#define MENTRY_LEN 9  /* number of magic index */

void magic_init();

void magic_fini();

bool magic_scan(int fd);

#endif /* SS_MAGIC_H */
