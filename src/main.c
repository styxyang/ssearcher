#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>

#include "options.h"

/* #define TEST_KMP */

void init_kmp_table(int *kmp_table, char *pattern, unsigned int pat_len) {
  /* if (pat_len < 1 */
  kmp_table[0] = 0;
  int i;

  for (i = 1; i < pat_len; i++) {
    if (pattern[kmp_table[i - 1]] == pattern[i]) {
      kmp_table[i] = kmp_table[i - 1] + 1;
    } else {
      kmp_table[i] = 0;
    }
  }
}

int kmp(char *text, int text_len, char *pattern, int pat_len) {
  int max_match = 0;
  int pat_idx = 0;

  int text_pos = 0;
  int found = 0;

  int *kmp_table = (int *)malloc(sizeof(int) * pat_len);
  init_kmp_table(kmp_table, pattern, pat_len);

  while (text_pos < text_len) {
    /* printf("%d %d %c %c\n", text_pos, pat_idx, text[text_pos], pattern[pat_idx]); */
    if (text[text_pos] == pattern[pat_idx]) {
      pat_idx++;
      text_pos++;
      if (pat_idx == pat_len)
        return text_pos - pat_len - 1;
      continue;
    } else {
      pat_idx = kmp_table[pat_idx - 1];
      if (text[text_pos] == pattern[pat_idx]) {
        pat_idx++;
        text_pos++;
        if (pat_idx == pat_len)
          return text_pos - pat_len - 1;
        continue;
      } else if (pat_idx != 0) {
        pat_idx = 0;
      } else {
        text_pos++;
      }
    }
  }
  return -1;
}

int main(int argc, char *argv[])
{
#ifdef TEST_KMP
  if (argc < 2)
    return 1;

  int fd = open(argv[1], O_RDONLY);
  char *p = mmap(0, 4096, PROT_READ, MAP_SHARED, fd, 0);
  /* printf("%d\n", p); */
  char str[16];
  snprintf(str, 10, "%s\n", p);
  printf("%s", str);
  char *pattern = "ab";

  int pos = kmp(p, pattern);
  printf("%d\n", pos);
  snprintf(str, 10, "%s\n", p+pos+1);
  printf("%s", str);
  munmap(p, strlen(p));
  close(fd);
  return 0;
#else
  parse_options(argc, argv);
  printf("%s\n", opt.search_pattern);

  /* default to search in the current directory */
  if (!opt.input_file) {
    err_exit("err_exit");
  }

  /* open file for searching
   * use mmap in the future */
  /* TODO: tilde-expansion ? */
  int fd;
  if ((fd = open(opt.input_file, O_RDONLY)) == -1) {
    err_exit("ss:fail to open file for read");
  }

  /* map the file to a space of 4K bytes */
  char *p;
  if ((p = mmap(0, 4096 * 1024, PROT_READ, MAP_SHARED, fd, 0)) == MAP_FAILED) {
    err_exit("ss: fail to map the file");
  }

  char str[16];                         /* to hold the strings about the loc of pos */
  char *pattern = opt.search_pattern;

  int text_len, pat_len;

  pat_len = strlen(pattern);
  if (p[4095])
    text_len = 4096 * 1024;
  else
    text_len = strlen(p);

  printf("%d %d\n", text_len, pat_len);
  /* start searching using KMP algorithm */
  int pos = kmp(p, text_len, pattern, pat_len);

  printf("%d\n", pos);
  if (pos > 0) {
    /* snprintf(str, 15, "%s\n", p+pos+1); */
    /* /\* printf("%sssss\n", str); *\/ */
    /* fprintf(stdout, "%ssssssssssss\n", str); */
    /* fflush(NULL); */
  }
  /* printf("ssssssssssssss\n"); */

  munmap(p, 4096 * 1024);
  close(fd);
  
  return 0;
#endif
}
