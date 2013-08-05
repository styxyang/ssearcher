#include "ss_config.h"
#include "ss_match.h"
#include <stdio.h>
#include <stdlib.h>

static void kmp_table_init(int *kmp_table, char *pattern, unsigned int pat_len) {

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
int32_t kmp_match(char *text, int text_len, char *pat, int pat_len, int *linum)
{
    /* int max_match = 0; */
    int pat_pos = 0;        /* index points to locations of pattern */
    int text_pos = 0;       /* index points to locations of text  */
    int line_cnt = *linum;

    /* array holding the prefix vector */
    int *kmp_table = NULL;
#ifdef SS_MALIGN
    if (posix_memalign((void **)&kmp_table, 0x1000, sizeof(int) * pat_len) != 0) {
        printf("can't memalign memory");
        exit(-1);
    }
#else
    /* FIXME should not always malloc if the pattern remains the same */
    if ((kmp_table = (int *)malloc(sizeof(int) * pat_len)) == NULL) {
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
    kmp_table_init(kmp_table, pat, pat_len);

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
    free(kmp_table);
    *linum = line_cnt;
    return -1;
found:
    /* would it be unnecessary to freeit?
     * since it will be only executed quickly and then terminated */
    free(kmp_table);
    *linum = line_cnt;
    return text_pos - pat_len + 1;
}
