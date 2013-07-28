#include "gtest/gtest.h"

extern "C" {
#include "ss_config.h"
#include "ss_trie.h"
}

using namespace std;

class TrieTest : public ::testing::Test
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

TEST_F(TrieTest, InsertOneElement)
{
    srand(time(NULL));
    int cnt = rand() % 20;
    uint8_t *array = (uint8_t *)malloc(sizeof(uint8_t) * cnt);

    int i;
    for (i = 0; i < cnt; i++) {
        array[i] = rand() % 16;
    }
    EXPECT_NE(trie_insert(tr, array, cnt), (void *)NULL);  // make sure the entry is inserted
    EXPECT_EQ(trie_insert(tr, array, cnt), (void *)NULL);  // can't insert same entry twice

    EXPECT_NE(trie_lookup(tr, array, cnt), (void *)NULL);  /* make sure it can be found */

    array[i-1]++;
    EXPECT_EQ(trie_lookup(tr, array, cnt), (void *)NULL);  /* can't found a modified entry */

    free(array);
}

TEST_F(TrieTest, SearchInMany)
{
    srand(time(NULL));
    const int nentries = 20;
    uint8_t *array[nentries];
    int cnts[nentries];

    int i, j;
    // insert entries into trie
    for (i = 0; i < nentries; i++) {
        int cnt = rand() % 20 + 1;  // random size of an entry
        array[i] = (uint8_t *)malloc(sizeof(uint8_t) * cnt);
        cnts[i] = cnt;
        for (j = 0; j < cnt; j++) {
            array[i][j] = rand() % 16;
        }
        EXPECT_NE(trie_insert(tr, array[i], cnt), (void *)NULL);
    }

    // query entries
    for (i = 0; i < nentries; i++) {
        EXPECT_NE(trie_lookup(tr, array[i], cnts[i]), (void *)NULL);
        free(array[i]);
    }
}
