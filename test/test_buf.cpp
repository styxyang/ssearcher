#include "gtest/gtest.h"

extern "C" {
#include "config.h"
#include "buffer.h"
#include <string.h>

extern __thread char  *buffer;
extern __thread size_t cap;
extern __thread size_t off;
}

#define DEFAULT_SIZE 4096

using namespace std;

class BufferTest : public ::testing::Test
{
protected:

    virtual void SetUp()
    {
        init_buffer();
    }

    virtual void TearDown()
    {
        destroy_buffer();
    }
};

TEST_F(BufferTest, SizeCheck)
{
    const char *gnu = "The GNU operating system is the Unix-like operating system,"
            "which is entirely free software,"
            " that we in the GNU Project have developed since 1984.";

    EXPECT_EQ(cap, DEFAULT_SIZE);
    EXPECT_EQ(off, 0);

    write_buffer(gnu, strlen(gnu));
    EXPECT_EQ(off, strlen(gnu));

    write_buffer(gnu, strlen(gnu));
    EXPECT_EQ(off, strlen(gnu) * 2);

    char page[DEFAULT_SIZE];
    write_buffer(page, DEFAULT_SIZE);
    EXPECT_EQ(off, 2 * strlen(gnu) + DEFAULT_SIZE);
    EXPECT_EQ(cap, DEFAULT_SIZE * 2);

    reset_buffer();
    EXPECT_EQ(off, 0);
    EXPECT_EQ(cap, DEFAULT_SIZE);

    char page1[DEFAULT_SIZE * 2];
    write_buffer(gnu, strlen(gnu));
    write_buffer(page1, DEFAULT_SIZE * 2);
    EXPECT_EQ(off, strlen(gnu) + DEFAULT_SIZE * 2);
    EXPECT_EQ(cap, DEFAULT_SIZE * 4);
}

TEST_F(BufferTest, ContentCheck)
{
    const char hello[] = "hello";
    const char world[] = "world";
    const char helloworld[] = "helloworld";

    write_buffer(hello, 5);
    EXPECT_EQ(off, 5);
    EXPECT_EQ(cap, DEFAULT_SIZE);
    EXPECT_EQ(strncmp(read_buffer(), hello, 5), 0);
    write_buffer(world, 5);
    EXPECT_EQ(cap, DEFAULT_SIZE);
    EXPECT_EQ(strncmp(read_buffer(), helloworld, 10), 0);
    EXPECT_EQ(off, 10);
}

TEST_F(BufferTest, WritelnTest)
{
    EXPECT_EQ(off, 0);
    const char line[] = "The quick brown fox jumps over the lazy dog";
    const char result[] = "The quick brown \e[48;5;220m\e[30;22mfox\e[0m jumps over the lazy dog";
    size_t nbytes = writeline_color_buffer(line, sizeof(result), 16, 3);
    EXPECT_EQ(cap, DEFAULT_SIZE);
    EXPECT_EQ(strncmp(read_buffer(), result, sizeof(result) - 1), 0);
    EXPECT_EQ(nbytes, sizeof(result) - 1);
    EXPECT_EQ(off, sizeof(result) - 1);
    printf("%s\n", read_buffer());
}

TEST_F(BufferTest, AmendTest)
{
    const char line[] = "The quick brown fox jumps over the lazy dog";
    const char result[] = "The quick brown \e[48;5;220m\e[30;22mfox\e[0m jumps over \e[48;5;220m\e[30;22mthe\e[0m lazy dog";
    size_t nbytes = writeline_color_buffer(line, sizeof(result), 16, 3);
    amendline_color_buffer(nbytes, 31, 3, 1);
    EXPECT_EQ(off, sizeof(result) - 1);
    EXPECT_EQ(strncmp(read_buffer(), result, sizeof(result) - 1), 0);
}
