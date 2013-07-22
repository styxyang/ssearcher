#ifndef SS_TRIE_H
#define SS_TRIE_H

#define MAX_CHILDREN 16

struct trie {
    int isleaf;
    struct trie *children[MAX_CHILDREN];
} __attribute__((aligned(64)));

struct trie *trie_init(struct trie **root);
struct trie *trie_insert(struct trie *root, int *hex, int size);
struct trie *trie_lookup(struct trie *root, int *hex, int size);
void trie_destroy(struct trie *root);

#endif /* SS_TRIE_H */
