#ifndef SS_OPTIONS_H
#define SS_OPTIONS_H

#include "debug.h"

#define SM_ALGO_KMP 0

typedef struct {
    int   str_matching_algo;
    char *search_pattern;
    char *input_file;
    char *input_dir;
} cli_options;

cli_options opt;

void parse_options(int argc, char *argv[]);

#endif  /* SS_OPTIONS_H */
