#include "ss_trie.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int main(int argc, char *argv[])
{
    struct trie *tr = NULL;

    trie_init(&tr);

    uint8_t arr0[] = {0x8, 0xf, 0x3};
    uint8_t arr1[] = {0x7, 0xD, 0xF};
    uint8_t arr2[] = {0x0, 0x0, 0xF};
    uint8_t arr3[] = {0xd, 0x4, 0x0};
    trie_insert(tr, arr0, 3);
    trie_insert(tr, arr1, 3);
    trie_insert(tr, arr2, 3);

    assert(trie_lookup(tr, arr0, 3) != NULL);
    assert(trie_lookup(tr, arr1, 3) != NULL);
    assert(trie_lookup(tr, arr2, 3) != NULL);
    assert(trie_lookup(tr, arr3, 3) == NULL);


    srand(time(NULL));
    int cnt = rand() % 20;
    uint8_t *array = (uint8_t *)malloc(sizeof(uint8_t) * cnt);

    int i;
    for (i = 0; i < cnt; i++) {
        array[i] = rand() % 16;
    }
    trie_insert(tr, array, cnt);
    assert(trie_insert(tr, array, cnt) == NULL);  /* can't insert same entry twice */

    assert(trie_lookup(tr, array, cnt));  /* make sure it can be found */

    array[i-1]++;
    assert(trie_lookup(tr, array, cnt) == NULL);  /* can't found a modified entry */

    free(array);

    trie_destroy(tr);
    return 0;
}
