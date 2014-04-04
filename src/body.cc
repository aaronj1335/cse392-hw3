#include <stdlib.h>
#include <stdio.h>

#include "body.h"
#include "util.h"

midlvl_t toMid(point_t p, lvl_t level) {
  mid_t m = 0;
  mid_t pointMask = 1;
  mid_t midMask = 1;
  size_t numBits = MORTON_BITS / D;
  mid_t max = ((mid_t) 1) << numBits;
  mid_t x = (mid_t) (p.x * max);
  mid_t y = (mid_t) (p.y * max);

  for (size_t i = 0; i < numBits; i++) {
    if (y & pointMask)
      m |= midMask;

    midMask = midMask << 1;

    if (x & pointMask)
      m |= midMask;

    midMask = midMask << 1;
    pointMask = pointMask << 1;
  }

  return midlvl(m, level);
}

