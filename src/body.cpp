#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <tbb/parallel_sort.h>

#include "body.h"
#include "util.h"

using namespace tbb;

struct comparator_t {
  comparator_t(midlvl_t const* const mids, const size_t n) : mids(mids), n(n) {}

  private:
    midlvl_t const* const mids;
    const size_t n;

  public:
    int operator()(size_t a, size_t b) const {
      return mids[a] < mids[b];
    }
};

midlvl_t toMid(point_t p, lvl_t level) {
  mid_t m = 0;
  mid_t pointMask = 1;
  mid_t midMask = 1;
  size_t numBits = MORTON_BITS / DIM;
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

void sortByMid(point_t const* const points, midlvl_t const* const mids,
    const size_t n, size_t* idxs) {

  #pragma omp parallel for
    for (size_t i = 0; i < n; i++)
      idxs[i] = i;

  parallel_sort(idxs, idxs + n, comparator_t(mids, n));
}

void partition(midlvl_t const* const mids, size_t const* const idxs,
    const size_t n, midlvl_t* partitions) {
  #pragma omp parallel for
    for (size_t i = 0; i < n; i++)
      partitions[i] = mids[idxs[i]];
}

