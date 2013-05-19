#include <unistd.h>
#include <getopt.h>
#include <stdio.h>

#include "options.h"

void usage() {
  printf("there is nothing\n");
}

void parse_options(int argc, char *argv[]) {
  int ch, opt_idx;

  struct option longopts[] = {
    { "kmp", no_argument, &ss_opts.str_matching_algo,0 }
  };

  while ((ch = getopt_long(argc, argv, "", longopts, &opt_idx)) != -1) {
    switch (ch) {
      case '?':
        usage();
      case 0:
        if (ss_opts.str_matching_algo == SM_ALGO_KMP)
          printf("using KMP algorithm\n");
        break;
      default:
        usage();
    }
  }
  argc -= optind;
  argv += optind;
}
