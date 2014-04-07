#ifndef __morton__
#define __morton__

#include <stdlib.h>

// this is used to represent the combined morton id and level info (and doesn't
// actually provide type safety!)
typedef uint64_t midlvl_t;

// this just represents the morton id w/o the level (and also likewise
// similarly as well does neither provide said type safety)
typedef uint64_t mid_t;

// this represents the level w/o the morton id
typedef uint8_t lvl_t;

#define BITS (sizeof(midlvl_t) * 8)

// dimension of the points
#define D 2

// number of children per tree node
#define NC (1 << D)

// number of bits to store the levels
#define LEVEL_BITS 6
#define LEVEL_MASK ((1 << LEVEL_BITS) - 1)
#define MAX_LEVEL ((BITS - LEVEL_BITS) / D)

#define MORTON_BITS (BITS - LEVEL_BITS)

// if we reserve 6 bits for the level, then we can actually use that to
// indicate a level for which the remaining 58 bits of the morton ID can't
// represent. so we'll use that invalid space to encode a "null" value
//
// ...also idk if this is actually useful
#define NULL_MID ((midlvl_t) ((1 << LEVEL_BITS) - 1))

lvl_t lvl(midlvl_t n);
mid_t mid(midlvl_t n);
midlvl_t midlvl(mid_t mid, lvl_t lvl);

bool isAncestor(midlvl_t ancestor, midlvl_t descendent);
bool isAncestor(mid_t anc, lvl_t ancLvl, mid_t desc, lvl_t descLvl);

midlvl_t parent(midlvl_t n);

mid_t childIndex(midlvl_t n);

bool couldHaveNextSibling(midlvl_t n);

// this assumes there is a next sibling
midlvl_t nextPossibleSibling(midlvl_t n);

// returns -1 if item not found
int binsrch(midlvl_t const* const tree, size_t l, midlvl_t needle);

void children(midlvl_t const* const tree, size_t l, size_t idx, int* sibs);
void siblings(midlvl_t const* const tree, size_t l, size_t idx, int* sibs);

midlvl_t leastCommonAncestor(midlvl_t n1, midlvl_t n2);

#endif
