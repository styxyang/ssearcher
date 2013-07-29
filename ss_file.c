#include "ss_file.h"
#include "ss_debug.h"

#include <sys/mman.h>
#include <sys/stat.h>
#include <assert.h>

static __thread int    tlsfd;
static __thread char  *tlsbuf;
static __thread size_t tlslen;

void *map_file(int fd)
{
    assert(fd);

    struct stat st;
    if (fstat(fd, &st) < 0) {
        die("fail to fstat");
    }

    if ((tlsbuf = mmap(0, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED) {
        die("fail to mmap");
    }

    tlsfd = fd;
    tlslen = st.st_size;
    return tlsbuf;
}

bool unmap_file(void *addr)
{
    assert(addr == tlsbuf);

    if (munmap(addr, tlslen) < 0) {
        perror("munmap");
        die("fail to unmap");
    }

    return true;
}
