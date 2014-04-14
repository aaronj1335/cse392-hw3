#include <iostream>
#include <iomanip>
#include <vector>

#include <assert.h>
#include <stdio.h>
#include <limits.h>

#include "morton.h"
#include "body.h"
#include "qtree.h"

using namespace std;

uint64_t fixture1[] = {0ull, 1ull, 2ull, 1152921504606846978ull,
  2305843009213693954ull, 3458764513820540930ull, 4611686018427387905ull,
  4611686018427387906ull, 5764607523034234882ull, 6917529027641081858ull,
  8070450532247928834ull, 13835058055282163713ull, 13835058055282163714ull,
  14987979559889010690ull, 16140901064495857666ull, 17293822569102704642ull};

uint64_t fixture2[] = {0ull, 1ull, 2ull, 1152921504606846978ull,
  2305843009213693954ull, 3458764513820540930ull, 4611686018427387905ull,
  4611686018427387906ull, 5764607523034234882ull, 6917529027641081858ull,
  8070450532247928834ull, 13835058055282163713ull, 16140901064495857666ull,
  17293822569102704642ull};

bool isEqual(int* a, int* b, size_t l) {
  bool eql = true;

  for (size_t i = 0; i < l; i++)
    eql = eql && a[i] == b[i];

  return eql;
}

bool sorted(midlvl_t* mids, size_t l, size_t* idxs) {
  bool sorted = true;
  bool allocateAndFreeIdxs = idxs == NULL;

  if (allocateAndFreeIdxs) {
    idxs = new size_t[l];
    #pragma omp parallel for
      for (size_t i = 0; i < l; i++)
        idxs[i] = i;
  }

  #pragma omp parallel for
    for (size_t i = 1; i < l; i++) {
      bool greater = mids[idxs[i]] >= mids[idxs[i - 1]];
      sorted = sorted && greater;
    }

  if (allocateAndFreeIdxs)
    delete idxs;

  return sorted;
}

bool sorted(midlvl_t* mids, size_t l) {
  return sorted(mids, l, NULL);
}

void init(int* a) {
  for (size_t i = 0; i < NUM_KIDS; i++)
    a[i] = INT_MAX;
}

int main(int argc, char* argv[]) {
  // ************************************************************
  cerr << "======================================== BITS" << endl;

  assert(BITS == 64);

  // ************************************************************
  cerr << "======================================== lvl_t and LEVEL_BITS" << endl;

  assert(sizeof(lvl_t) * 8 >= LEVEL_BITS);

  // ************************************************************
  cerr << "======================================== lvl()" << endl;

  assert(lvl(0) == 0);
  assert(lvl(1) == 1);
  assert(lvl(62) == 62);
  assert(lvl(63) == 63);
  assert(lvl(64) == 0);
  assert(lvl(65) == 1);
  assert(lvl(127) == 63);
  assert(lvl(128) == 0);
  assert(lvl(769) == 1);

  // ************************************************************
  cerr << "======================================== mid()" << endl;

  assert(mid(0) == 0);
  assert(mid(1) == 0);
  assert(mid(62) == 0);
  assert(mid(63) == 0);
  assert(mid(64) == 1);
  assert(mid(65) == 1);
  assert(mid(127) == 1);
  assert(mid(128) == 2);

  // ************************************************************
  cerr << "======================================== midlvl()" << endl;

  assert(midlvl(0, 63) == 63);
  assert(midlvl(1, 0) == 64);
  assert(midlvl(1, 63) == 127);
  assert(midlvl(2, 0) == 128);

  // ************************************************************
  cerr << "======================================== isAncestor()" << endl;

  assert(isAncestor(0, 3458764513820540930ull));
  assert(isAncestor(1, 3458764513820540930ull));
  assert(!isAncestor(2, 3458764513820540930ull));

  assert(isAncestor(0, 13835058055282163713ull));
  assert(isAncestor(13835058055282163713ull, 17293822569102704642ull));
  assert(!isAncestor(17293822569102704642ull, 13835058055282163713ull));

  assert(!isAncestor(17293822569102704642ull, 3458764513820540930ull));
  assert(!isAncestor(3458764513820540930ull, 13835058055282163713ull));
  assert(!isAncestor(3458764513820540930ull, 17293822569102704642ull));

  // ************************************************************
  cerr << "======================================== parent()" << endl;

  assert(parent(2ull) == 1ull);
  assert(parent(3458764513820540930ull) == 1ull);
  assert(parent(1ull) == 0ull);

  assert(parent(17293822569102704642ull) == 13835058055282163713ull);
  assert(parent(13835058055282163713ull) == 0ull);

  // ************************************************************
  cerr << "======================================== childIndex()" << endl;

  assert(childIndex(1) == 0);
  assert(childIndex(2) == 0);
  assert(childIndex(1152921504606846978ull) == 1);
  assert(childIndex(2305843009213693954ull) == 2);
  assert(childIndex(13835058055282163714ull) == 0);
  assert(childIndex(14987979559889010690ull) == 1);

  // ************************************************************
  cerr << "======================================== couldHaveNextSibling()" << endl;

  assert(couldHaveNextSibling(2));
  assert(couldHaveNextSibling(1152921504606846978ull));
  assert(couldHaveNextSibling(2305843009213693954ull));
  assert(!couldHaveNextSibling(3458764513820540930ull));

  // ************************************************************
  cerr << "======================================== nextPossibleSibling()" << endl;

  assert(nextPossibleSibling(2) == 1152921504606846978ull);
  assert(nextPossibleSibling(1152921504606846978ull) == 2305843009213693954ull);
  assert(nextPossibleSibling(2305843009213693954ull) == 3458764513820540930ull);
  assert(nextPossibleSibling(1) == 4611686018427387905ull);

  // ************************************************************
  cerr << "======================================== binsrch()" << endl;

  midlvl_t a1[] = {0, 1, 2, 3, 4};
  assert(binsrch(a1, 5, 2) == 2);
  assert(binsrch(a1, 5, 0) == 0);
  assert(binsrch(a1, 5, 4) == 4);
  assert(binsrch(a1, 5, 6) == -1);

  midlvl_t a2[] = {0, 1, 2, 3, 4, 5};
  assert(binsrch(a2, 6, 2) == 2);
  assert(binsrch(a2, 6, 0) == 0);
  assert(binsrch(a2, 6, 4) == 4);
  assert(binsrch(a2, 6, 6) == -1);

  // ************************************************************
  cerr << "======================================== children()" << endl;

  int v[] ={INT_MAX, INT_MAX, INT_MAX, INT_MAX};
  int expected1[] = {7, 8, 9, 10};
  children(fixture1, 16, 6, v);
  assert(isEqual(v, expected1, NUM_KIDS));

  int expected2[] = {1, 6, -1, 11};
  init(v);
  children(fixture1, 16, 0, v);
  assert(isEqual(v, expected2, NUM_KIDS));

  int expected3[] = {-1, -1, 12, 13};
  init(v);
  children(fixture2, 14, 11, v);
  assert(isEqual(v, expected3, NUM_KIDS));

  // ************************************************************
  cerr << "======================================== siblings()" << endl;

  init(v);
  siblings(fixture1, 16, 7, v);
  assert(isEqual(v, expected1, NUM_KIDS));

  init(v);
  siblings(fixture1, 16, 8, v);
  assert(isEqual(v, expected1, NUM_KIDS));

  init(v);
  siblings(fixture1, 16, 9, v);
  assert(isEqual(v, expected1, NUM_KIDS));

  init(v);
  siblings(fixture1, 16, 10, v);
  assert(isEqual(v, expected1, NUM_KIDS));

  init(v);
  siblings(fixture1, 16, 6, v);
  assert(isEqual(v, expected2, NUM_KIDS));

  init(v);
  siblings(fixture2, 14, 12, v);
  assert(isEqual(v, expected3, NUM_KIDS));

  // ************************************************************
  cerr << "======================================== leastCommonAncestor()" << endl;

  assert(leastCommonAncestor(2, 1152921504606846978ull) == 1);
  assert(leastCommonAncestor(1, 2) == 0);
  assert(leastCommonAncestor(2, 1) == 0);
  assert(leastCommonAncestor(2, 4611686018427387905ull) == 0);

  // ************************************************************
  cerr << "======================================== toMid()" << endl;

  point_t p(0.5, 0.5);
  assert(toMid(p, 1) == 13835058055282163713ull);

  // ************************************************************
  cerr << "======================================== sortByMid()" << endl;

  size_t l = 1 << 20;
  point_t*  points = new point_t[l];
  midlvl_t* mids = new midlvl_t[l];
  size_t*   idxs = new size_t[l];

  #pragma omp parallel for
    for (size_t i = 0; i < l; i++) {
      points[i].x = ((double) rand()) / ((double) RAND_MAX);
      points[i].y = ((double) rand()) / ((double) RAND_MAX);
      mids[i] = toMid(points[i], 1);
    }

  assert(!sorted(mids, l));
  sortByMid(points, mids, l, idxs);
  assert(sorted(mids, l, idxs));

  // ************************************************************
  cerr << "======================================== QTree" << endl;

  l = 1 << 1;
  points = new point_t[l];
  mids = new midlvl_t[l];
  idxs = new size_t[l];

  for (size_t i = 0; i < l; i++) {
    points[i].x = ((double) rand()) / ((double) RAND_MAX);
    points[i].y = ((double) rand()) / ((double) RAND_MAX);
    mids[i] = toMid(points[i], 1);
  }
  sortByMid(points, mids, l, idxs);

  QTree tree(points);
  tree.insert(idxs, l);

  assert(&tree);

  // ************************************************************
  cerr << "======================================== Testing QTree::iterator" << endl;

  l = 1 << 10;
  points = new point_t[l];
  mids = new midlvl_t[l];
  idxs = new size_t[l];

  for (size_t i = 0; i < l; i++) {
    points[i].x = ((double) rand()) / ((double) RAND_MAX);
    points[i].y = ((double) rand()) / ((double) RAND_MAX);
    mids[i] = toMid(points[i], 1);
  }
  sortByMid(points, mids, l, idxs);

  QTree tree4(points);
  midlvl_t prev = 0;
  for (size_t i = 0; i < l; i++) {
    tree4.insert(idxs[i]);
    for (QTree::iterator it = tree4.begin(); it != tree4.end(); it++)
      // will assert(false) if we call it->toMid() on empty child
      it->toMid();
  }

  l = 1 << 4;
  points = new point_t[l];
  mids = new midlvl_t[l];
  idxs = new size_t[l];

  for (size_t i = 0; i < l; i++) {
    points[i].x = ((double) rand()) / ((double) RAND_MAX);
    points[i].y = ((double) rand()) / ((double) RAND_MAX);
    mids[i] = toMid(points[i], 1);
  }
  sortByMid(points, mids, l, idxs);

  QTree tree5(points);
  for (size_t i = 0; i < l; i++)
    tree5.insert(idxs[i]);
  QTree::iterator it = tree5.begin();
  prev = it->toMid();

  for (; it != tree5.end(); it++) {
    assert(it->toMid() >= prev);
    prev = it->toMid();
  }

  // ************************************************************
  cerr << "======================================== subtreeSize" << endl;

  // idx             morton id QTree
  // ---  -------------------- --------------------------------------------------

  //   0                     0 QTree<0x7fff58b4a3f0, 0, P (0, 0)>
  //   1                     1 QTree<0x7fc6b9e1d4a0, 1, P (0, 0)>
  //   2                     2 QTree<0x7fc6b9e1d620, 2, P (0, 0)>
  //   3    224485332902455875 QTree<0x7fc6b9e1d7a0, 3, L (0.0899074, 0.0728559)>
  //   4    536303309992939715 QTree<0x7fc6b9e1d800, 3, L (0.241987, 0.0781364)>
  //   5   3458764513820540930 QTree<0x7fc6b9e1d740, 2, P (0.25, 0.25)>
  //   6   3458764513820540931 QTree<0x7fc6b9e1d920, 3, P (0.25, 0.25)>
  //   7   3484793516036766788 QTree<0x7fc6b9e1daa0, 4, L (0.307982, 0.259371)>
  //   8   3576809027682800900 QTree<0x7fc6b9e1db00, 4, L (0.318576, 0.301914)>
  //   9   4430335732812156419 QTree<0x7fc6b9e1da40, 3, L (0.48988, 0.405702)>
  //  10   4611686018427387905 QTree<0x7fc6b9e1d500, 1, P (0.5, 0)>
  //  11   4842340074169815298 QTree<0x7fc6b9e1dc20, 2, L (0.584999, 0.0834366)>
  //  12   6884107963633161090 QTree<0x7fc6b9e1dc80, 2, L (0.943668, 0.227026)>
  //  13   6917529027641081858 QTree<0x7fc6b9e1dce0, 2, P (0.5, 0.25)>
  //  14   6917529027641081859 QTree<0x7fc6b9e1dda0, 3, P (0.5, 0.25)>
  //  15   6989586621679009796 QTree<0x7fc6b9e1df80, 4, P (0.5625, 0.25)>
  //  16   7003943150365545733 QTree<0x7fc6b9e1e0a0, 5, L (0.582333, 0.269646)>
  //  17   7057194964474447365 QTree<0x7fc6b9e1e1c0, 5, L (0.610775, 0.297957)>
  //  18   9223372036854775809 QTree<0x7fc6b9e1d560, 1, P (0, 0.5)>
  //  19  12489503624292354818 QTree<0x7fc6b9e1e2e0, 2, L (0.238763, 0.881891)>
  //  20  13267274223964235266 QTree<0x7fc6b9e1e340, 2, L (0.276723, 0.889071)>
  //  21  13835058055282163713 QTree<0x7fc6b9e1d5c0, 1, P (0.5, 0.5)>
  //  22  13835058055282163714 QTree<0x7fc6b9e1e3a0, 2, P (0.5, 0.5)>
  //  23  14411518807585587203 QTree<0x7fc6b9e1e5e0, 3, P (0.5, 0.625)>
  //  24  14627691589699371012 QTree<0x7fc6b9e1e7c0, 4, P (0.5625, 0.6875)>
  //  25  14651617888815502725 QTree<0x7fc6b9e1e880, 5, L (0.62133, 0.687648)>
  //  26  14686087203322870853 QTree<0x7fc6b9e1e940, 5, L (0.608897, 0.731815)>
  //  27  14744968145691858435 QTree<0x7fc6b9e1e640, 3, L (0.625613, 0.675141)>
  //  28  15348654668895592130 QTree<0x7fc6b9e1e400, 2, L (0.943446, 0.501017)>
  //  29  17306860877768997314 QTree<0x7fc6b9e1e4c0, 2, L (0.76092, 0.777598)>

  double rawPoints[32] = {0.582333, 0.269646, 0.943668, 0.227026, 0.62133,
    0.687648, 0.307982, 0.259371, 0.241987, 0.0781364, 0.238763, 0.881891,
    0.943446, 0.501017, 0.584999, 0.0834366, 0.318576, 0.301914, 0.276723,
    0.889071, 0.608897, 0.731815, 0.610775, 0.297957, 0.76092, 0.777598,
    0.0899074, 0.0728559, 0.48988, 0.405702, 0.625613, 0.675141};
  points = (point_t*) rawPoints;
  for (size_t i = 0; i < l; i++) {
    mids[i] = toMid(points[i], 1);
  }
  sortByMid(points, mids, l, idxs);

  QTree tree6(points);
  for (size_t i = 0; i < l; i++)
    tree6.insert(idxs[i]);
  vector<midlvl_t> fullTree;
  for (QTree::iterator it = tree6.begin(); it != tree6.end(); it++)
    fullTree.push_back(it->toMid());

  assert(subtreeSize(&fullTree[0], fullTree.size(), 0) == 29);
  assert(subtreeSize(&fullTree[0], fullTree.size(), 1) == 8);
  assert(subtreeSize(&fullTree[0], fullTree.size(), 2) == 2);
  assert(subtreeSize(&fullTree[0], fullTree.size(), 3) == 0);
  assert(subtreeSize(&fullTree[0], fullTree.size(), 5) == 4);
  assert(subtreeSize(&fullTree[0], fullTree.size(), 11) == 0);
  assert(subtreeSize(&fullTree[0], fullTree.size(), 12) == 0);

  return 0;
}

