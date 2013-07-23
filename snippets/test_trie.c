#include "trie.h"

#include <stdio.h>
#include <assert.h>

int main(int argc, char *argv[])
{
    struct trie *tr = NULL;

    trie_init(&tr);

    int arr0[] = {0x8, 0xf, 0x3};
    int arr1[] = {0x7, 0xD, 0xF};
    int arr2[] = {0x0, 0x0, 0xF};
    int arr3[] = {0xd, 0x4, 0x0};
    trie_insert(tr, arr0, 3);
    trie_insert(tr, arr1, 3);
    trie_insert(tr, arr2, 3);

    assert(trie_lookup(tr, arr0, 3) != NULL);
    assert(trie_lookup(tr, arr1, 3) != NULL);
    assert(trie_lookup(tr, arr2, 3) != NULL);
    assert(trie_lookup(tr, arr3, 3) == NULL);

    trie_destroy(tr);
    return 0;
}
