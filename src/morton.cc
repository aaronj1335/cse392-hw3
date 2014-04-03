#include <assert.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>

#include "morton.h"

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

lvl_t lvl(midlvl_t n) {
  return LEVEL_MASK & n;
}

mid_t mid(midlvl_t n) {
  return n >> LEVEL_BITS;
}

midlvl_t midlvl(mid_t mid, lvl_t lvl) {
  return (mid << LEVEL_BITS) | lvl;
}

bool isAncestor(midlvl_t anc, midlvl_t desc) {
  return isAncestor(mid(anc), lvl(anc), mid(desc), lvl(desc));
}

bool isAncestor(mid_t anc, lvl_t ancLvl, mid_t desc, lvl_t descLvl) {
  return ancLvl < descLvl && (anc & desc) == anc;
}

midlvl_t parent(midlvl_t n) {
  lvl_t l = lvl(n);
  unsigned int shift = MORTON_BITS - (D * (l - 1));
  return midlvl(mid(n) >> shift << shift, l - 1);
}

