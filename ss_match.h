#ifndef SS_MATCH_H
#define SS_MATCH_H

#define SS_MALIGN

void kmp_prepare(const char *pattern, unsigned int pat_len);
int32_t kmp_match(const char *text, int text_len, const char *pat, int pat_len, uint32_t *linum);
void kmp_finish();

#endif /* SS_MATCH_H */
