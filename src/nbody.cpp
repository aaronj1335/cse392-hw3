#include <iostream>

#include <getopt.h>
#include <stdlib.h>
#include <assert.h>

#include <omp.h>

#include "qtree.h"
#include "util.h"

using namespace std;

bool smallTest = false;

size_t getTid() {
  int tid = omp_get_thread_num();

  assert(tid >= 0);

  return (size_t) tid;
}

size_t getP() {
  int p = omp_get_num_threads();

  assert(p > 0);

  return (size_t) p;
}

void nbody(point_t const* const points, const size_t l, float* u) {
  midlvl_t* mids = new midlvl_t[l];
  size_t* idxs = new size_t[l];

  for (size_t i = 0; i < l; i++)
    mids[i] = toMid(points[i], 1);

  sortByMid(mids, l, idxs);

  size_t p;
  QTree** trees;
  size_t* treeSizes;

  #pragma omp parallel
  {
    size_t tid = getTid();

    if (!tid) {
      p = getP();
      trees = new QTree*[p];
      treeSizes = new size_t[p];
    }
  }

  #pragma omp parallel
  {
    size_t tid = getTid();
    size_t tl = l / p;

    trees[tid] = new QTree(points);
    trees[tid]->insert(&idxs[tl * tid], tl);

    treeSizes[tid] = 0;

    for (QTree::iterator i = trees[tid]->begin(); i != trees[tid]->end(); i++)
      treeSizes[tid]++;
  }

  size_t tl = 0; // tree length (total length of all trees)
  for (size_t i = 0; i < p; i++)
    tl += treeSizes[i];

  midlvl_t* treeMids = new midlvl_t[tl];

  #pragma omp parallel
  {
    size_t tid = getTid();
    size_t myStart = 0;

    for (size_t i = 0; i < tid; i++)
      myStart += treeSizes[i];

    size_t offset = 0;

    for (QTree::iterator i = trees[tid]->begin(); i != trees[tid]->end(); i++)
      treeMids[myStart + ++offset] = i->toMid();
  }

  if (smallTest) {
    for (size_t i = 0; i < p; i++) {
      cout << "==================== thread " << i << " tree" << endl;
      _print_hierarchy(*trees[i]);
    }
  }

  delete[] mids;
  delete[] idxs;
  for (size_t i = 0; i < p; i++)
    delete trees[i];
  delete[] trees;
  delete[] treeSizes;
  delete[] treeMids;
}

int main(int argc, char* argv[]) {
  size_t l = 1 << 10;
  int opt;

  while ((opt = getopt(argc, argv, "n:s")) != -1) {
    switch (opt) {
      case 'n': // problem size
        if (!smallTest)
          l = atoi(optarg);
        break;
      case 's': // just do a small test
        smallTest = true;
        l = 16;
        break;
      default: /* '?' */
        cerr << "USAGE: " << argv[0]
          << " [-n number-of-points]"
          << " [-s]"
          << endl;
        exit(EXIT_FAILURE);
    }
  }

  point_t* points = new point_t[l];
  float* u = new float[l];

  if (smallTest) {
    // same as in test.cpp
    float rawPoints[(1 << 4) * 3] = {0.582333, 0.269646, 0.1, 0.943668,
      0.227026, 0.1, 0.62133, 0.687648, 0.1, 0.307982, 0.259371, 0.1, 0.241987,
      0.0781364, 0.1, 0.238763, 0.881891, 0.1, 0.943446, 0.501017, 0.1,
      0.584999, 0.0834366, 0.1, 0.318576, 0.301914, 0.1, 0.276723, 0.889071,
      0.1, 0.608897, 0.731815, 0.1, 0.610775, 0.297957, 0.1, 0.76092, 0.777598,
      0.1, 0.0899074, 0.0728559, 0.1, 0.48988, 0.405702, 0.1, 0.625613,
      0.675141, 0.1};

    points = (point_t*) rawPoints;

  } else {
    for (size_t i = 0; i < l; i++) {
      points[i].x = ((float) rand()) / ((float) RAND_MAX);
      points[i].y = ((float) rand()) / ((float) RAND_MAX);
      points[i].weight = (float) rand();
    }
  }

  nbody(points, l, u);

  cout << "generating problem of size " << l << endl;

  return 0;
}
