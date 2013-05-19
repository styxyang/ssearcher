#ifndef SS_OPTIONS_H
#define SS_OPTIONS_H

#define SM_ALGO_KMP 0

typedef struct {
  int str_matching_algo;
} cli_options;

cli_options ss_opts;

void parse_options(int argc, char *argv[]);

#endif  /* SS_OPTIONS_H */
