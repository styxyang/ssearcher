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

    read(fd, buf, 4);
    printf("%x\n", *(unsigned int *)buf);


    close(fd);
    return 0;
}
