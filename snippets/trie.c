#include "trie.h"

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

    /* deal with the last node */
}

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
    return node;
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

int main(int argc, char *argv[])
{
    struct trie *tr = NULL;

    trie_init(&tr);

    int arr[] = {1, 2, 3};
    trie_insert(tr, arr, 3);

    assert(trie_lookup(tr, arr, 3) != NULL);

    trie_destroy(tr);
    return 0;
}
