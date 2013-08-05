#include "ss_options.h"
#include "ss_debug.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>

#include <fcntl.h>

cli_options opt;
static const char *program_name;

static void print_usage()
{
    fprintf(stdout,
            "Usage: %s [OPTION] pattern\n", program_name);
    fprintf(stdout,
            "  -h  --help             Display this usage information.\n"
            "  -o  --output filename  Write output to file.\n"
            "  -v  --verbose          Print verbose messages.\n");
    exit(1);
}

void parse_options(int argc, char *argv[]) {
    int ch, opt_idx;
    program_name = argv[0];

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
                print_usage();
            case 0:
                dprintf(INFO, "%d, %d\n", optopt, optind);
                if (strcmp(longopts[opt_idx].name, "file") == 0) {
                    opt.input_file = optarg;
                } else if (strcmp(longopts[opt_idx].name, "dir") == 0) {
                    opt.input_dir = optarg;
                } else if (opt.str_matching_algo == SM_ALGO_KMP) {
                    printf("using KMP algorithm\n");
                }
                break;
            default:
                print_usage();
        }
    }

    /* `optind' is the index of next argument to be processed
     * Suppress options already parsed */
    /* argc -= optind; */
    /* argv += optind; */

    if (optind >= argc) {
        dprintf(ERROR, "parse_options: missing pattern string");
        print_usage();
    }
    opt.search_pattern = argv[optind];
    printf("search for \"%s\"-%d\n", opt.search_pattern, strlen(opt.search_pattern));

    if (!opt.input_file && !opt.input_dir) {
        dprintf(ERROR, "parse_options: missing search scope");
        printf("default in current directory\n");
        if (!opt.search_pattern) {
            print_usage();
        }
    }
}
