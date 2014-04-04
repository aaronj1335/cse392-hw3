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
  mid_t parentMid = mid(n) >> shift << shift;

  return midlvl(parentMid, l - 1);
}

mid_t childIndex(midlvl_t n) {
  lvl_t l = lvl(n);
  unsigned int parentBits = (l - 1) * D;

  return mid(n << parentBits >> (parentBits + MORTON_BITS - D * l));
}

bool couldHaveNextSibling(midlvl_t n) {
  mid_t idx = childIndex(n);

  return idx < (1 << D) - 1;
}

midlvl_t nextPossibleSibling(midlvl_t n) {
  lvl_t l = lvl(n);

  // i don't understand why i need to separate these two calls
  mid_t siblingMask = ((1 << D) - 1);
  siblingMask = ~(siblingMask << (MORTON_BITS - (l * D)));

  mid_t idx = (childIndex(n) + 1) << (MORTON_BITS - (l * D));
  mid_t m = (mid(n) & siblingMask) | idx;

  return midlvl(m, l);
}

int binsrch(midlvl_t const* const tree, size_t l, midlvl_t needle) {
  size_t cur, start = 0, end = l - 1;

  while (end >= start) {
    cur = start + (end - start) / 2;
    if (needle == tree[cur])
      return cur;
    else if (needle > tree[cur])
      start = cur + 1;
    else
      end = cur - 1;
  }

  return -1;
}

void siblings(midlvl_t const* const tree, size_t l, size_t idx, int* sibs) {
  midlvl_t m = tree[idx];
  midlvl_t p = parent(m);
  int pIdx = binsrch(tree, idx + 1, p);
  int cIdx = pIdx + 1;

  if (pIdx < 0 || !isAncestor(p, tree[cIdx])) {
    for (size_t i = 0; i < NC; i++)
      sibs[i] = -1;
    return;
  }

  size_t sibIdx = childIndex(tree[cIdx]);

  for (size_t i = 0; i < sibIdx; i++)
    sibs[i] = -1;

  sibs[sibIdx] = cIdx;
  midlvl_t nexPossibleChild = tree[cIdx];

  do {
    nexPossibleChild = nextPossibleSibling(nexPossibleChild);
    cIdx = binsrch(&tree[pIdx], l - pIdx, nexPossibleChild);
    sibIdx++;

    sibs[sibIdx] = cIdx >= 0? pIdx + cIdx : -1;

  } while (couldHaveNextSibling(nexPossibleChild));
}

midlvl_t leastCommonAncestor(midlvl_t n1, midlvl_t n2) {
  midlvl_t cur = n1;

  while (!isAncestor(cur, n1) || !isAncestor(cur, n2))
    cur = parent(cur);

  return cur;
}
