#include <assert.h>
#include <omp.h>
#include <math.h>

#include "qtree.h"
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

float radius(const point_t& p1, const point_t& p2) {
  float deltaX = p1.x - p2.x;
  float deltaY = p1.y - p2.y;

  return sqrt(deltaX * deltaX + deltaY * deltaY);
}

float potential(midlvl_t const* const mids, QTree const* const* const nodes,
    point_t* const* const points, const size_t l, const size_t idx,
    const point_t& p) {
  float g = 0;

  if (nodes[idx]->isLeafNode() || nodes[idx]->isWellSeparatedFrom(p)) {
    point_t const* const nodePoint = points[idx];
    float r = radius(*nodePoint, p);
    g = -1 / 2 / 3.14159265358979f * log(r);
  } else {
    int kids[] = {-1, -1, -1, -1};

    children(mids, l, idx, kids);

    for (size_t i = 0; i < NUM_KIDS; i++)
      if (kids[i] >= 0)
        g += potential(mids, nodes, points, l, kids[i], p);
  }

  return g;
}

