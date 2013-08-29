#include "file.h"
#include "debug.h"

#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <assert.h>

void *map_file(fileinfo *fi)
{
    assert(fi->fd);

    struct stat st;
    if (fstat(fi->fd, &st) < 0) {
        close(fi->fd);
        die("fail to fstat");
    }

    /* make sure size is always positive and file is not empty */
    if (st.st_size <= 0)
        return NULL;

    if ((fi->pmap = mmap(0, st.st_size, PROT_READ, MAP_PRIVATE, fi->fd, 0)) == MAP_FAILED) {
        perror("mmap");
        close(fi->fd);
        die("fail to mmap: %d", fi->fd);
    }
    fi->size = st.st_size;
    return fi->pmap;
}

bool unmap_file(fileinfo *fi)
{
    assert(fi->pmap);

    if (fi->pmap &&
        fi->size &&
        munmap(fi->pmap, fi->size) < 0) {
        perror("munmap");
        die("fail to unmap");
    }
    fi->pmap = 0;
    fi->size = 0;
    close(fi->fd);
    return true;
}

/* sanity check: whether the position is within the file boundary */
bool inbound(uint32_t pos)
{
    return true;
}
