#ifndef __morton__
#define __morton__

#include <stdint.h>

// this is used to represent the combined morton id and level info
typedef uint64_t midlvl_t;

// this just represents the morton id w/o the level
typedef uint64_t mid_t;

// this represents the level w/o the morton id
typedef uint8_t lvl_t;

#define BITS (sizeof(midlvl_t) * 8)

// dimension of the points
#define D 2

// number of children per tree node
#define NC 2 << D

// number of bits to store the levels
#define LEVEL_BITS 6
#define LEVEL_MASK ((1 << LEVEL_BITS) - 1)

#define MORTON_BITS (BITS - LEVEL_BITS)

lvl_t lvl(midlvl_t n);
mid_t mid(midlvl_t n);
midlvl_t midlvl(mid_t mid, lvl_t lvl);

bool isAncestor(midlvl_t ancestor, midlvl_t descendent);
bool isAncestor(mid_t anc, lvl_t ancLvl, mid_t desc, lvl_t descLvl);

midlvl_t parent(midlvl_t n);

#endif
