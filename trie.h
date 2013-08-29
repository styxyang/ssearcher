#ifndef SS_TRIE_H
#define SS_TRIE_H

#include "config.h"

#define MAX_CHILDREN 256

struct trie {
    bool isleaf;
    /* value is unnecessary for ssearcher */
    struct trie *children[MAX_CHILDREN];
} __attribute__((aligned(64)));

struct trie *trie_init(struct trie **root);
struct trie *trie_insert(struct trie *root, uint8_t *target, int size);
struct trie *trie_lookup(struct trie *root, uint8_t *target, int size);

/*
 * provide target with size usually much longer than magic numbers
 * in the host that the caller need not match the exact length
 */
struct trie *trie_scan(struct trie *root, uint8_t *target, int size);
void trie_destroy(struct trie *root);

#endif /* SS_TRIE_H */
