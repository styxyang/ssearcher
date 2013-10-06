#ifndef SS_UTIL_H
#define SS_UTIL_H

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

/**
 * list structure from linux source
 */
struct list_head {
    struct list_head *next, *prev;
};

#define LIST_HEAD_INIT(name) { &(name), &(name) }
#define LIST_HEAD(name) struct list_head name = LIST_HEAD_INIT(name)

#define container_of(ptr, type, member) ({                      \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - offsetof(type,member) );})

#define list_entry(ptr, type, member)           \
    container_of(ptr, type, member)

static inline void INIT_LIST_HEAD(struct list_head *list)
{
    list->next = list;
    list->prev = list;
}

static inline void __list_add(struct list_head *new,
                              struct list_head *prev,
                              struct list_head *next)
{
    next->prev = new;
    new->next = next;
    new->prev = prev;
    prev->next = new;
}

static inline int list_empty(const struct list_head *head)
{
    return head->next == head;
}

static inline void list_add(struct list_head *new, struct list_head *head)
{
    __list_add(new, head, head->next);
}

static inline void list_add_tail(struct list_head *new, struct list_head *head)
{
    __list_add(new, head->prev, head);
}

/**
 * list_for_each_entry  -       iterate over list of given type
 * @pos:        the type * to use as a loop cursor.
 * @head:       the head for your list.
 * @member:     the name of the list_struct within the struct.
 */
#define list_for_each_entry(pos, head, member)                          \
    for (pos = list_entry((head)->next, typeof(*pos), member);          \
         &pos->member != (head);                                        \
         pos = list_entry(pos->member.next, typeof(*pos), member))

#endif /* SS_UTIL_H */
