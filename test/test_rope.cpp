#include "gtest/gtest.h"

extern "C" {
#include "config.h"
#include "buffer.h"
#include <string.h>
}

#define DEFAULT_SIZE 4096

using namespace std;

class RopeTest : public ::testing::Test
{
protected:

    buffer buf;

    virtual void SetUp()
    {
        buf_init(&buf);
    }

    virtual void TearDown()
    {
        buf_destroy(&buf);
    }
};

TEST_F(RopeTest, AddTest)
{
    buf_write(&buf, "helloworld", 10, TAG_DEFAULT, 0);
    buf_write(&buf, "helloworld", 10, TAG_DEFAULT, 0);
    buf_write(&buf, "helloworld", 10, TAG_DEFAULT, 0);
    EXPECT_EQ(list_empty(&buf.ropelist), false);
}

TEST_F(RopeTest, TrivialTest)
{
    EXPECT_EQ(list_empty(&buf.ropelist), true);
}

TEST_F(RopeTest, DumpTest)
{
    buf_write(&buf, "helloworld", 10, TAG_DEFAULT, 0);
    buf_write(&buf, "helloworld", 10, TAG_DEFAULT, 0);
    buf_write(&buf, "helloworld", 10, TAG_DEFAULT, 0);
    buf_dump(&buf);
    EXPECT_EQ(1, 1);
}
