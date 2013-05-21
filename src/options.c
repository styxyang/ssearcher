#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>

#include "options.h"

void usage() {
  printf("there is nothing\n");
}

void parse_options(int argc, char *argv[]) {
  int ch, opt_idx;

  struct option longopts[] = {
    { "kmp", no_argument, &ss_opts.str_matching_algo,0 },
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
        DEBUG_HERE
#endif
        if (ss_opts.str_matching_algo == SM_ALGO_KMP)
          printf("using KMP algorithm\n");
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
    perror("missing pattern");
    exit(1);
  }
  ss_pat = argv[optind];
  printf("search for %s\n", ss_pat);
}
