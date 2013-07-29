#include "ss_file.h"
#include "ss_debug.h"

#include <sys/mman.h>
#include <sys/stat.h>
#include <assert.h>

static __thread int      tlsfd;
static __thread char    *tlsbuf;
static __thread uint32_t tlslen;

void *map_file(int fd)
{
    assert(fd);

    struct stat st;
    if (fstat(fd, &st) < 0) {
        die("fail to fstat");
    }

    if (st.st_size <= 0)
        return NULL;

    if ((tlsbuf = mmap(0, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED) {
        perror("mmap");
        die("fail to mmap: %d", fd);
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

/* sanity check: whether the position is within the file boundary */
inline bool inbound(uint32_t pos)
{
    return (pos<tlslen);
}

inline uint32_t map_len(int fd)
{
    assert(fd == tlsfd);
    return tlslen;
}
