#ifndef _REALBOGGLE_CPP_
#define _REALBOGGLE_CPP_

#include "RealBoard.h"
#include <stdlib.h>

static const char die[16][8] = {
  "xlderi",
  "twoota",
  "fpsakf",
  "zhrlnn",
  "hneegw",
  "ttrely",
  "eiunes",
  "ahspco",
  "sioste",
  "dvyrle",
  "umcoit",
  "rwtehv",
  "oabbjo",
  "ngeeaa",
  "tstiyd",
  "nuihmq"};

/*static double rand_dbl() {
  //return ((double)mt.BRandom()) / 4294967296.0;
  return mt.Random();
}*/

RealBoard::RealBoard(TRandomMersenne* trm) {
  // Make sure we stay random
  //mt->BRandom_stir();
  //printf("%f\n", mt->Random());
  mt = trm;

  // Come up with a random permutation
  for (int i=0; i<16; i++) perm[i] = i;
  for (int i=0; i<16; i++) {
    int idx = mt->BRandom() % (16-i);
    if (idx) {
      int swp = perm[i];
      perm[i] = perm[i+idx];
      perm[i+idx] = swp;
    }
  }

  for (int i=0; i<16; i++)
    rolls[i] = mt->BRandom()%6;

  for (int i=0; i<16; i++)
    lets[i] = die[perm[i]][rolls[i]];
  lets[16] = '\0';
}

string RealBoard::pretty_str() {
  string ret;
  for (int i=0; i<16; i++) {
    ret += lets[i];
    if (i%4==3) ret += "\n";
    else ret += " ";
  }
  return ret;
}
void RealBoard::doSwap() {
  int d1 = mt->BRandom() % 16;
  int d2 = mt->BRandom() % 16;
  int tmp = perm[d1];
  perm[d1] = perm[d2];
  perm[d2] = tmp;
  //printf("swapped %d and %d\n", d1, d2);

  tmp = rolls[d1];
  rolls[d1] = rolls[d2];
  rolls[d2] = tmp;

  lets[d1] = die[perm[d1]][rolls[d1]];
  lets[d2] = die[perm[d2]][rolls[d2]];
}

void RealBoard::doReroll() {
  int d = mt->IRandom(0,15); //mt->BRandom() % 16;
  int r = mt->IRandom(0, 5); //mt->BRandom() % 6;
  //printf("reroll die %d to %d, was %d\n", d, r, rolls[d]);
  rolls[d] = r;
  lets[d] = die[perm[d]][r];
}

void RealBoard::mutate(double pctReroll, double pctSwap) {
  // First one is "free"
  if (mt->Random()*(pctReroll+pctSwap) <= pctReroll) {
    doReroll();
  } else {
    doSwap();
  }

  // Now do others at the expected percentage
  while (mt->Random() < pctReroll) {
    doReroll();
  }

  while (mt->Random() < pctSwap) {
    doSwap();
  }
}

ostream& operator<<(ostream&o, const RealBoard& b) {
  o << b.c_str();
  return o;
}

#endif /* _REALBOGGLE_CPP_ */
