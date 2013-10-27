#ifndef SS_MATCH_H
#define SS_MATCH_H

#define SS_MALIGN

void kmp_prepare(const char *pattern, size_t pat_len);
bool kmp_match(const char *text, size_t text_len, uint32_t *linum, uint32_t *matchpos);
void kmp_finish();

#endif /* SS_MATCH_H */
