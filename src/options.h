#ifndef SS_OPTIONS_H
#define SS_OPTIONS_H

#include "debug.h"
#include "config.h"

#define SM_ALGO_KMP 0

typedef struct {
    int   str_matching_algo;
    char *search_pattern;
    int   search_patlen;
    char *input_file;
    char **input_dirs;
    bool  list_matching_files;
} cli_options;

extern cli_options opt;

void parse_options(int argc, char *argv[]);

#endif /* SS_OPTIONS_H */
