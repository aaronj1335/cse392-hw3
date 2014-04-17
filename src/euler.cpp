#include <omp.h>

#include "euler.h"

void eulerTour(midlvl_t const* const mids, const size_t l, size_t *const inIdxs,
    size_t *const outIdxs) {

  #pragma omp parallel for
    for (size_t i = 0; i < l; i++) {
      lvl_t lv = lvl(mids[i]);
      size_t inIdx = i * 2 - (size_t) lv;
      size_t subtree = subtreeSize(mids, l, i);
      size_t outIdx = inIdx + 2 * subtree + 1;
      inIdxs[i] = inIdx;
      outIdxs[i] = outIdx;
    }
}

