#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <mach-o/fat.h>
#include <mach-o/loader.h>

int main(int argc, char *argv[]) {
    int fd;
    char buf[16];
    struct fat_header hdr;

    if (argc != 2) {
        printf("wrong number of arguments");
        exit(1);
    }

    if ((fd = open(argv[1], O_RDONLY)) == -1) {
        perror("failed to open file");
        exit(1);
    }
    if (read(fd, &hdr, sizeof(hdr)) < 0) {
        perror("read error");
    }
    printf("%x\n", (hdr.magic == MH_MAGIC_64));
    /* for (i = 0; i < sizeof(buf); i++) */
    /*     printf("%d ", buf[i] & 0xff); */
    close(fd);
    return 0;
}
