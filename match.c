#include "config.h"
#include "match.h"
#include <stdio.h>
#include <stdlib.h>

static __thread char const  *pat;
static __thread size_t       pat_len;
static __thread int         *kmp_table = NULL;

void kmp_prepare(const char *pattern, size_t pattern_length)
{
    pat     = pattern;
    pat_len = pattern_length;
#ifdef SS_MALIGN
    if (kmp_table == NULL &&
        posix_memalign((void **)&kmp_table, 0x1000,
                       sizeof(int) * pat_len) != 0) {
        printf("can't memalign memory");
        exit(-1);
    }
#else
    /* FIXME should not always malloc if the pattern remains the same */
    if (kmp_table == NULL && (kmp_table = (int *)malloc(sizeof(int) * pat_len)) == NULL) {
        printf("can't to malloc memory");
        exit(-1);
    }
#endif
#ifdef SS_MADV
    if (madvise(kmp_table, sizeof(int) * pat_len,
                MADV_WILLNEED | MADV_ZERO_WIRED_PAGES) < 0) {
        perror("fail to madvise");
    }
#endif

    kmp_table[0] = 0;
    int i;

    for (i = 1; i < pat_len; i++) {
        if (pattern[kmp_table[i - 1]] == pattern[i]) {
            kmp_table[i] = kmp_table[i - 1] + 1;
        } else {
            kmp_table[i] = 0;
        }
    }
}

/* use KMP algorithm to do string matching
 * `linum' is the current line number and will increase
 * with the process of matching */
bool kmp_match(const char *text, size_t text_len,
               uint32_t *linum, uint32_t *matchpos)
{
    /* int max_match = 0; */
    int pat_pos = 0;        /* index points to locations of pattern */
    int text_pos = 0;       /* index points to locations of text  */
    int line_cnt = *linum;

    while (text_pos < text_len) {
        if (text[text_pos] == '\n')
            line_cnt++;

        if (text[text_pos] == pat[pat_pos]) {
            if (pat_pos == pat_len - 1)
                goto found;
            else {
                pat_pos++;
                text_pos++;
            }
        } else {
            /* fallback to the position of proper prefix
             * which is also the suffix of the pattern */
            if (pat_pos == 0) {
                text_pos++;
                continue;
            }
            pat_pos = kmp_table[pat_pos - 1];
            if (text[text_pos] == pat[pat_pos]) {
                if (pat_pos == pat_len - 1) {
                    goto found;
                } else {
                    pat_pos++;
                    text_pos++;
                }
            } else if (pat_pos != 0) {
                /* fallback fail, start from beginning */
                pat_pos = 0;
            } else {
                text_pos++;
            }
        }
    }
    *linum = line_cnt;
    *matchpos = -1L;
    return false;
found:
    /* would it be unnecessary to freeit?
     * since it will be only executed quickly and then terminated */
    *linum = line_cnt;
    *matchpos = text_pos - pat_len + 1;
    return true;
}

void kmp_finish()
{
    if (kmp_table != NULL) {
        free(kmp_table);
        kmp_table = NULL;
    }
}
