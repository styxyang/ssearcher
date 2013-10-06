#include "gtest/gtest.h"

extern "C" {
#include "config.h"
#include "util.h"
#include "magic.h"
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
    int cnt = rand() % 20 + 1;
    uint8_t *array = (uint8_t *)malloc(sizeof(uint8_t) * cnt);

    int i;
    for (i = 0; i < cnt; i++) {
        array[i] = rand() % 256;
    }
    EXPECT_NE(trie_insert(tr, array, cnt), (void *)NULL);  // make sure the entry is inserted
    EXPECT_EQ(trie_insert(tr, array, cnt), (void *)NULL);  // can't insert same entry twice

    EXPECT_NE(trie_lookup(tr, array, cnt), (void *)NULL);  /* make sure it can be found */

    array[cnt-1] += 2;
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
        trie_insert(tr, array[i], cnt);
    }

    // query entries
    for (i = 0; i < nentries; i++) {
        EXPECT_NE(trie_lookup(tr, array[i], cnts[i]), (void *)NULL);
        free(array[i]);
    }
}

TEST_F(TrieTest, SearchMagic)
{
    int i;
    static uint8_t magic_index[][MENTRY_LEN + 1] = {
        {0x00, 0x00, 0x00, 0x14, 0x66, 0x74, 0x79, 0x70, 8},
        {0x00, 0x00, 0x00, 0x18, 0x66, 0x74, 0x79, 0x70, 8},
        {0x00, 0x00, 0x00, 0x20, 0x66, 0x74, 0x79, 0x70, 8},
        {0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 4},
        {0x1F, 0x9D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 2},
        {0x1F, 0xA0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 2},
        {0x25, 0x50, 0x44, 0x46, 0x00, 0x00, 0x00, 0x00, 4},
        {0x38, 0x42, 0x50, 0x53, 0x00, 0x00, 0x00, 0x00, 4},
        {0x42, 0x4D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 2},
        {0x43, 0x44, 0x30, 0x30, 0x31, 0x00, 0x00, 0x00, 5},
        {0x47, 0x49, 0x46, 0x38, 0x37, 0x61, 0x00, 0x00, 6},
        {0x47, 0x49, 0x46, 0x38, 0x39, 0x61, 0x00, 0x00, 6},
        {0x49, 0x44, 0x33, 0x00, 0x00, 0x00, 0x00, 0x00, 3},
        {0x49, 0x49, 0x2A, 0x00, 0x00, 0x00, 0x00, 0x00, 4},
        {0x4D, 0x4D, 0x00, 0x2A, 0x00, 0x00, 0x00, 0x00, 4},
        {0x4D, 0x5A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 2},
        {0x4F, 0x67, 0x67, 0x53, 0x00, 0x00, 0x00, 0x00, 4},
        {0x50, 0x4B, 0x03, 0x04, 0x00, 0x00, 0x00, 0x00, 4},
        {0x50, 0x4B, 0x05, 0x06, 0x00, 0x00, 0x00, 0x00, 4},
        {0x50, 0x4B, 0x07, 0x08, 0x00, 0x00, 0x00, 0x00, 4},
        {0x52, 0x61, 0x72, 0x21, 0x1A, 0x07, 0x00, 0x00, 7},
        {0x52, 0x61, 0x72, 0x21, 0x1A, 0x07, 0x01, 0x00, 8},
        {0x66, 0x4C, 0x61, 0x43, 0x00, 0x00, 0x00, 0x00, 4},
        {0x7F, 0x45, 0x4C, 0x46, 0x00, 0x00, 0x00, 0x00, 4},
        {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 8},
        {0xCA, 0xFE, 0xBA, 0xBE, 0x00, 0x00, 0x00, 0x00, 4},
        {0xCE, 0xFA, 0xED, 0xFE, 0x00, 0x00, 0x00, 0x00, 4},
        {0xCF, 0xFA, 0xED, 0xFE, 0x00, 0x00, 0x00, 0x00, 4},
        {0xD0, 0xCF, 0x11, 0xE0, 0x00, 0x00, 0x00, 0x00, 4},
        {0xEF, 0xBB, 0xBF, 0x00, 0x00, 0x00, 0x00, 0x00, 3},
        {0xFE, 0xED, 0xFA, 0xCE, 0x00, 0x00, 0x00, 0x00, 4},
        {0xFE, 0xED, 0xFA, 0xCF, 0x00, 0x00, 0x00, 0x00, 4},
        {0xFF, 0xFB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 2},
        {0xFF, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 2},
        {0xFF, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 4}
    };

    for (i = 0; i < sizeof(magic_index) / (MENTRY_LEN + 1) / sizeof(uint8_t); i++) {
        EXPECT_NE(trie_insert(tr, &magic_index[i][0], magic_index[i][8]), (void *)NULL);
    }
    uint8_t array0[] = {0x52, 0x61, 0x72, 0x21, 0x1A, 0x07, 0x01, 0x00};
    EXPECT_NE(trie_lookup(tr, array0, 8), (void *)NULL);
    uint8_t array1[] = {0x52, 0x61, 0x72, 0x21, 0x1A, 0x07, 0x01, 0x10};
    EXPECT_EQ(trie_lookup(tr, array1, 8), (void *)NULL);
    uint8_t array2[] = {0x52, 0x60, 0x72, 0x21, 0x1A, 0x07, 0x01, 0x00};
    EXPECT_EQ(trie_lookup(tr, array2, 8), (void *)NULL);
    uint8_t array3[] = {0x52, 0x61, 0x72, 0x21, 0x1A, 0x07, 0x01};
    EXPECT_EQ(trie_lookup(tr, array3, 7), (void *)NULL);
}

TEST_F(TrieTest, ScanTest)
{
    uint8_t array[] = {0x52, 0x61, 0x72, 0x21, 0x1A, 0x07, 0x01, 0x00};
    trie_insert(tr, array, 8);

    uint8_t array0[] = {0x52, 0x61, 0x72, 0x21, 0x1A, 0x07, 0x01};
    EXPECT_EQ(trie_scan(tr, array0, 7), (void *)NULL);

    uint8_t array1[] = {0x52, 0x61, 0x72, 0x21, 0x1A, 0x07};
    EXPECT_EQ(trie_scan(tr, array1, 6), (void *)NULL);

    uint8_t array2[] = {0x52, 0x61, 0x72, 0x22, 0x1A, 0x07, 0x01, 0x00};
    EXPECT_EQ(trie_scan(tr, array2, 8), (void *)NULL);

    uint8_t array3[] = {0x52, 0x61, 0x72, 0x21, 0x1A, 0x07, 0x01, 0x00, 0x11};
    EXPECT_NE(trie_scan(tr, array3, 9), (void *)NULL);
}
