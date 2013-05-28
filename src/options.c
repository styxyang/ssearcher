#include "options.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>

#include <fcntl.h>

void usage() {
  printf("there is nothing\n");
}

void parse_options(int argc, char *argv[]) {
  int ch, opt_idx;

  struct option longopts[] = {
    { "kmp",  no_argument,       &opt.str_matching_algo, 0 },
    { "file", required_argument, NULL,                   0 },
    { "dir",  required_argument, NULL,                   0 }
  };

  /* since the arguments are not so many
   * I can still cope with them without checking whether opt_idx
   * has changed. Unchanged opt_idx means the opt is a short opt */
  while ((ch = getopt_long(argc, argv, "", longopts, &opt_idx)) != -1) {
    switch (ch) {
      case '?':
        usage();
      case 0:
#ifdef SS_DEBUG
        printf("%d, %d\n", optopt, optind);
        DEBUG_HERE;
#endif
        if (strcmp(longopts[opt_idx].name, "file") == 0) {
          opt.input_file = optarg;
        } else if (strcmp(longopts[opt_idx].name, "dir") == 0) {
          opt.input_dir = optarg;
        } else if (opt.str_matching_algo == SM_ALGO_KMP) {
          printf("using KMP algorithm\n");
        }
        break;
      default:
        usage();
    }
  }

  /* `optind' is the index of next argument to be processed
   * Suppress options already parsed */
  /* argc -= optind; */
  /* argv += optind; */

  if (optind >= argc) {
    /* fprintf(stderr, "missing pattern missing"); */
    err_exit("parse_options: missing pattern string");
  }
  opt.search_pattern = argv[optind];
  printf("search for %s\n", opt.search_pattern);

  if (!opt.input_file && !opt.input_dir) {
    /* fprintf(stderr, "currently only one input file is supported: %s", opt.input_file); */
    /* err_exit("parse_options: currently only one input file is supported"); */
    err_exit("parse_options: missing search scope");
  }
}
