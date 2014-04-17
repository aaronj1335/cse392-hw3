#ifndef __euler__
#define __euler__

#include "morton.h"

void eulerTour(midlvl_t const* const mids, const size_t l, size_t *const inIdxs,
    size_t *const outIdxs);

void parallelPrefixSum(double* data, const size_t l);

#endif
