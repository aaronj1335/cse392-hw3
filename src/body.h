#ifndef __body__
#define __body__

#include "morton.h"

// points are assumed to be between 0 and 1
struct point_t {
  double x;
  double y;
};

midlvl_t toMid(point_t p, lvl_t level);


#endif
