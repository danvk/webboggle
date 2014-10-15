#ifndef _REALBOGGLE_H_
#define _REALBOGGLE_H_

#include "randomc/randomc.h"
#include <ostream>
using namespace std;

class RealBoard {
  public:
  	RealBoard() : mt(NULL) {}
    RealBoard(TRandomMersenne*);

    inline const char* c_str() const { return lets; }
    string pretty_str();

    void mutate(double pctReroll, double pctSwap);

  private:
    int rolls[16];
    int perm[16];
    char lets[17];
    TRandomMersenne* mt;

    void doSwap();
    void doReroll();

}; // RealBoggle

ostream& operator<<(ostream& o, const RealBoard& b);

#endif /* _REALBOGGLE_H_ */
