#include "trie.h"
#include "../ss_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

struct trie *trie_init(struct trie **root)
{
    if (*root == NULL) {
        *root = (struct trie *)malloc(sizeof(struct trie));
        memset(*root, 0, sizeof(struct trie));
        return *root;
    }
    /* if root is not empty, then should free all the memory */
    return *root;
}

struct trie *trie_insert(struct trie *root, int *hex, int size)
{
    assert(size > 0);
    assert(hex != NULL);

    int i = 0;
    struct trie *node = root;
    for (; i < size; i++) {
        int idx = hex[i];
        if (node->children[idx] == NULL) {
            trie_init(&node->children[idx]);
        }
        node = node->children[idx];
    }
    node->isleaf = true;

    /* deal with the last node */
    return node->isleaf?node:NULL;
}

/* could be reused in trie_insert with a bit of whether allocating memory */
struct trie *trie_lookup(struct trie *root, int *hex, int size)
{
    assert(root != NULL);
    assert(size > 0);
    assert(hex != NULL);

    int i;
    struct trie *node = root;
    for (i = 0; i < size; i++) {
        int idx = hex[i];
        if (node->children[idx] == NULL)
            return NULL;
        node = node->children[idx];
    }
    return node->isleaf?node:NULL;
}

void trie_destroy(struct trie *root)
{
    int i;
    for (i = 0; i < MAX_CHILDREN; i++) {
        if (root->children[i] != NULL)
            trie_destroy(root->children[i]);
    }
    free(root);
}

/*****************/
/* Example usage */
/*****************/

/* int main(int argc, char *argv[]) */
/* { */
/*     struct trie *tr = NULL; */

/*     trie_init(&tr); */

/*     int arr0[] = {0x8, 0xf, 0x3}; */
/*     int arr1[] = {0x7, 0xD, 0xF}; */
/*     int arr2[] = {0x0, 0x0, 0xF}; */
/*     int arr3[] = {0xd, 0x4, 0x0}; */
/*     trie_insert(tr, arr0, 3); */
/*     trie_insert(tr, arr1, 3); */
/*     trie_insert(tr, arr2, 3); */

/*     assert(trie_lookup(tr, arr0, 3) != NULL); */
/*     assert(trie_lookup(tr, arr1, 3) != NULL); */
/*     assert(trie_lookup(tr, arr2, 3) != NULL); */
/*     assert(trie_lookup(tr, arr3, 3) == NULL); */

/*     trie_destroy(tr); */
/*     return 0; */
/* } */
