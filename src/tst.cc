#include <assert.h>
#include <stdio.h>
#include <limits.h>

#include "morton.h"

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

bool equal(int* a, int* b, size_t l) {
  bool eql = true;

  for (size_t i = 0; i < l; i++)
    eql = eql && a[i] == b[i];

  return eql;
}

void init(int* a) {
  for (size_t i = 0; i < NC; i++)
    a[i] = INT_MAX;
}

int main(int argc, char* argv[]) {
  // ************************************************************
  // BITS
  assert(BITS == 64);

  // ************************************************************
  // lvl_t and LEVEL_BITS
  assert(sizeof(lvl_t) * 8 >= LEVEL_BITS);

  // ************************************************************
  // level()
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
  // mid()
  assert(mid(0) == 0);
  assert(mid(1) == 0);
  assert(mid(62) == 0);
  assert(mid(63) == 0);
  assert(mid(64) == 1);
  assert(mid(65) == 1);
  assert(mid(127) == 1);
  assert(mid(128) == 2);

  // ************************************************************
  // midlvl()
  assert(midlvl(0, 63) == 63);
  assert(midlvl(1, 0) == 64);
  assert(midlvl(1, 63) == 127);
  assert(midlvl(2, 0) == 128);

  // ************************************************************
  // isAncestor()
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
  // parent()
  assert(parent(2ull) == 1ull);
  assert(parent(3458764513820540930ull) == 1ull);
  assert(parent(1ull) == 0ull);

  assert(parent(17293822569102704642ull) == 13835058055282163713ull);
  assert(parent(13835058055282163713ull) == 0ull);

  // ************************************************************
  // childIndex()
  assert(childIndex(1) == 0);
  assert(childIndex(2) == 0);
  assert(childIndex(1152921504606846978ull) == 1);
  assert(childIndex(2305843009213693954ull) == 2);
  assert(childIndex(13835058055282163714ull) == 0);
  assert(childIndex(14987979559889010690ull) == 1);

  // ************************************************************
  // couldHaveNextSibling()
  assert(couldHaveNextSibling(2));
  assert(couldHaveNextSibling(1152921504606846978ull));
  assert(couldHaveNextSibling(2305843009213693954ull));
  assert(!couldHaveNextSibling(3458764513820540930ull));

  // ************************************************************
  // nextPossibleSibling()
  assert(nextPossibleSibling(2) == 1152921504606846978ull);
  assert(nextPossibleSibling(1152921504606846978ull) == 2305843009213693954ull);
  assert(nextPossibleSibling(2305843009213693954ull) == 3458764513820540930ull);
  assert(nextPossibleSibling(1) == 4611686018427387905ull);

  // ************************************************************
  // binsrch()
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
  // siblings()
  int v[] ={INT_MAX, INT_MAX, INT_MAX, INT_MAX};
  int expected1[] = {7, 8, 9, 10};
  siblings(fixture1, 16, 7, v);
  assert(equal(v, expected1, NC));

  init(v);
  siblings(fixture1, 16, 8, v);
  assert(equal(v, expected1, NC));

  init(v);
  siblings(fixture1, 16, 9, v);
  assert(equal(v, expected1, NC));

  init(v);
  siblings(fixture1, 16, 10, v);
  assert(equal(v, expected1, NC));

  int expected2[] = {1, 6, -1, 11};
  init(v);
  siblings(fixture1, 16, 6, v);
  assert(equal(v, expected2, NC));

  int expected3[] = {-1, -1, 12, 13};
  init(v);
  siblings(fixture2, 14, 12, v);
  assert(equal(v, expected3, NC));

  return 0;
}

