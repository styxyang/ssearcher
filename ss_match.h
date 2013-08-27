#ifndef SS_MATCH_H
#define SS_MATCH_H

#define SS_MALIGN

int32_t kmp_match(const char *text, int text_len, const char *pat, int pat_len, uint32_t *linum);
void kmp_finish();

#endif /* SS_MATCH_H */
