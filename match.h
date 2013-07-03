#ifndef SS_MATCH_H
#define SS_MATCH_H

#define SS_MALIGN

#ifndef USE_PTHREAD
int kmp_match(char *text, int text_len, char *pat, int pat_len);
#else
void *kmp_match(void *a);
#endif

#endif /* SS_MATCH_H */
