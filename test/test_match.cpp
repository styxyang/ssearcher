#include "gtest/gtest.h"

extern "C" {
#include "ss_config.h"
#include "ss_match.h"
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
}

#define die(msg)                                \
    {                                           \
        perror(msg);                            \
        exit(1);                                \
    }

using namespace std;

class MatchTest : public ::testing::Test
{
protected:
    int fd;
    char *p;
    struct stat st;

    virtual void SetUp()
    {
        if ((fd = open("test/test_match.cpp", O_RDONLY)) < 0)
            die("open");

        if(fstat(fd, &st) < 0)
            die("fstat");

        if ((p = (char *)mmap(0, st.st_size, PROT_READ, MAP_SHARED, fd, 0)) == MAP_FAILED)
            die("mmap");
    }

    virtual void TearDown()
    {
        munmap(p, st.st_size);
        close(fd);
    }
};

TEST_F(MatchTest, KMPCheck)
{
    const char *pattern = "stat";

    uint32_t linum;
    int pstart = 0;
    int pos = kmp_match(p + pstart, strlen(p), pattern, 4, &linum);
    EXPECT_EQ(strncmp(pattern, p + pos + pstart, 4), 0);

    pstart = pos + 4;
    pos = kmp_match(p + pstart, strlen(p), pattern, 4, &linum);
    EXPECT_EQ(strncmp(pattern, p + pos + pstart, 4), 0);
}
