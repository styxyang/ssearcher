#ifndef SS_OPTIONS_H
#define SS_OPTIONS_H

#define SS_DEBUG 0

#define SM_ALGO_KMP 0

typedef struct {
  int str_matching_algo;
} cli_options;

cli_options ss_opts;

const char *ss_pat;

void parse_options(int argc, char *argv[]);

#endif  /* SS_OPTIONS_H */
