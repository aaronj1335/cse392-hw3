#include <iostream>
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

void _print_hierarchy(QTree& t) {
  size_t idx = 0;
  size_t indent = 0;
  QTree::iterator it = t.begin();
  lvl_t lastLvl = it->lvl();

  for (; it != t.end(); it++) {
    if (it->lvl() > lastLvl)
      indent += it->lvl() - lastLvl;

    if (it->lvl() < lastLvl)
      indent -= lastLvl - it->lvl();

    lastLvl = it->lvl();
    std::cout.width(3);
    std::cout << idx++ << " ";

    for (size_t i = 0; i < indent*4; i++)
      std::cout << " ";

    std::cout << *it << std::endl;
  }

}
