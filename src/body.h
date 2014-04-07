#ifndef __body__
#define __body__

#include "morton.h"

// points are assumed to be between 0 and 1
struct point_t {
  double x;
  double y;
  point_t() {}
  point_t(double x, double y) : x(x), y(y) {}
};

midlvl_t toMid(point_t p, lvl_t level);

void sortByMid(point_t const* const points, midlvl_t const* const mids,
    size_t* idxs, size_t n);


#endif
