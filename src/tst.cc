#include <assert.h>
#include <stdio.h>

#include "morton.h"

using namespace std;

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

  return 0;
}

