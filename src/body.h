#ifndef __body__
#define __body__

#include <ostream>

#include "morton.h"

using namespace std;

// if we were doing a well-software-engineered OOP design, we'd make a Point
// class w/ a Point#toMid() method, an overloaded `>` operator for sorting, a
// PointSet class w/ a PointSet#partition() method, etc.
//
// but we only care about perf, so in the interest of locality we'll just have
// arrays of points, morton id's, and then sort a list of indices using a
// custom comparator

// points are assumed to be between 0 and 1
struct point_t {
  float x;
  float y;
  float weight;

  point_t() {}
  point_t(float x, float y, float weight) : x(x), y(y), weight(weight) {}
  point_t(const point_t& p) : x(p.x), y(p.y), weight(p.weight) {}

  point_t& operator=(const point_t& rhs);

  point_t& operator+=(const point_t& rhs);
  const point_t operator+(const point_t& rhs) const;

  point_t& operator-=(const point_t& rhs);
  const point_t operator-(const point_t& rhs) const;

  friend ostream& operator<<(ostream& os, const point_t& p);
};

midlvl_t toMid(point_t p, lvl_t level);

// sort a list of points by their morton id
//
// input: arrays of points and morton id's such that points[i] <=> mids[i]
// output: the sorted list of length `n` in the form of a list of indices, such
// that idxs[0] is the index of points/mids that comes first in the sorted list
// of morton ids
void sortByMid(point_t const* const points, midlvl_t const* const mids,
    const size_t n, size_t* idxs);

// partition sorted points into a contiguous array
//
// this probably isn't necessary, but the data layout is so confusing i figure
// it doesn't hurt.
//
// input: the morton id's (unsorted) and the sorted list of their indices
// output: array of length `n` of the sorted morton id's such that
// localMids[0:n/numThreads] would be the first thread's morton id's
void partition(midlvl_t const* const mids, size_t const* const idxs,
    const size_t n, midlvl_t* localMids);

#endif
