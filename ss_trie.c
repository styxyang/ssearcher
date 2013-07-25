#include "ss_config.h"
#include "ss_trie.h"

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

static struct trie *trie_walk(struct trie *root, uint8_t *target, int size, bool create)
{
    int i = 0;
    struct trie *node = root;
    for (; i < size; i++) {
        int idx = target[i];
        if (node->children[idx] == NULL) {
            if (create) {
                trie_init(&node->children[idx]);
            } else {
                return NULL;
            }
        }
        node = node->children[idx];
    }

    if (create) {
        if (node->isleaf)
            return NULL;
        node->isleaf = true;
    }
    return node->isleaf?node:NULL;
}

struct trie *trie_insert(struct trie *root, uint8_t *target, int size)
{
    assert(root);
    assert(size > 0);
    assert(target);
 
    return trie_walk(root, target, size, true);
    /* deal with the last node */
}

/* could be reused in trie_insert with a bit of whether allocating memory */
struct trie *trie_lookup(struct trie *root, uint8_t *target, int size)
{
    assert(root);
    assert(size > 0);
    assert(target);

    return trie_walk(root, target, size, false);
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
