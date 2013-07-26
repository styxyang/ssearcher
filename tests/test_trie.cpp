#include "gtest/gtest.h"

extern "C" {
    #include "ss_config.h"
    #include "ss_trie.h"
}

using namespace std;

class TrieTest : public testing::Test
{
  protected:
    struct trie *tr;

    TrieTest(): tr(NULL) {}

    virtual void SetUp()
    {
        trie_init(&tr);
    }

    virtual void TearDown()
    {
        trie_destroy(tr);
    }
};

TEST_F(TrieTest, insertelement)
{
    srand(time(NULL));
    int cnt = rand() % 20;
    uint8_t *array = (uint8_t *)malloc(sizeof(uint8_t) * cnt);

    int i;
    for (i = 0; i < cnt; i++) {
        array[i] = rand() % 16;
    }
    trie_insert(tr, array, cnt);
    EXPECT_EQ(trie_insert(tr, array, cnt), (void *)NULL);  /* can't insert same entry twice */

    // assert(trie_lookup(tr, array, cnt));  /* make sure it can be found */

    // array[i-1]++;
    // assert(trie_lookup(tr, array, cnt) == NULL);  /* can't found a modified entry */

    free(array);
}
