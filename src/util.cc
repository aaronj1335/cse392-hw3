#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "util.h"

void _print_bin(uint64_t n) {
  char s[] = "0000000000000000000000000000000000000000000000000000000000000000";
  int i = strlen(s);
  long long unsigned mask = 0x1;

  assert(i == 64);

  while (--i >= 0) {
    s[i] = n & mask? '1' : '0';
    mask <<= 1;
  }

  printf("0b%s\n", s);
}

