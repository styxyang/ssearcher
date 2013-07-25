#ifndef SS_TRIE_H
#define SS_TRIE_H

#include "ss_config.h"

#define MAX_CHILDREN 16

struct trie {
    bool isleaf;
    /* value is unnecessary for ssearcher */
    struct trie *children[MAX_CHILDREN];
} __attribute__((aligned(64)));

struct trie *trie_init(struct trie **root);
struct trie *trie_insert(struct trie *root, uint8_t *target, int size);
struct trie *trie_lookup(struct trie *root, uint8_t *target, int size);
void trie_destroy(struct trie *root);

#endif /* SS_TRIE_H */
