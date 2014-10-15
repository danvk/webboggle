#include "RealBoard.h"
#include "trie.h"
#include "objBoggle.h"

#include <iostream>
#include <sys/time.h>
using namespace std;

const int nThreads = 2;
const double MAX_ROLL_MUT = 0.25;
const double MAX_SWAP_MUT = 0.25;
unsigned int numGrads = 0;
bool quitSig = false;

static double millis();

struct thread_arg {
  Boggler* b;
  int N;           // Thread number
  int nGrads;      // number of full gradient runs to use
  int stall;       // Number of generations to wait before restarting
  int stallm;      // Minimum stall
  int maxSc;
  RealBoard best;  // Best board ever found by this thread
};

void* gradient_loop(void* arg) {
  struct thread_arg* ta = (struct thread_arg*)arg;
  TRandomMersenne mt(random());

  unsigned int nGrads = ta->nGrads;
  Boggler* b = ta->b;
  int stallmax = ta->stall;
  int stallmin = ta->stallm;
  ta->maxSc = 0;
  while (!quitSig && numGrads<nGrads) { // do some load-balancing
    int g = ++numGrads;
    int gen = 0;
    int lastChange = 0;
    int maxScore = 0;
    int maxGap = 0;
    int numChanges = 0;
    double rollMut = MAX_ROLL_MUT * mt.Random();
    double swapMut = MAX_SWAP_MUT * mt.Random();
    int stall = 100*mt.IRandom(stallmin/100, stallmax/100);
    RealBoard bd(&mt);
    while (lastChange < stall) {
      gen++;
      lastChange++;
      RealBoard newb(bd);
      newb.mutate(rollMut, swapMut);
      int sc = b->board_score(newb.c_str());
      if (sc > maxScore) {
	maxScore = sc;
	bd = newb;
	maxGap = max(lastChange, maxGap);
	lastChange = 0;
	numChanges++;
      }
    }
    char summary[255];
    sprintf(summary, "%d: %d gen, mg=%d, nc=%d, rmut=%f, smut=%f, stall=%d: %s (%d)",
	g, gen, maxGap, numChanges, rollMut, swapMut, stall, bd.c_str(),
	maxScore);
    cout << summary << endl;

    if (maxScore > ta->maxSc) {
      ta->maxSc = maxScore;
      ta->best = bd;
    }
  }
  return NULL;
}

// Wait for the user to hit "quit"
void* quit_loop(void* arg) {
  string msg;
  while (cin >> msg) {
    if (msg == "quit") {
      quitSig=true;
      return NULL;
    } else {
      cout << "Unknown message: '" << msg << "'" << endl;
    }
  }
  return NULL;
}

// Usage: gradient [number of grdients to run [stall time for each]]
int main(int argc, char** argv) {
  unsigned int nGrads = 20;
  int stall = 2000;
  int stallm = 500;
  srandom(time(NULL));
  Trie* dict = new Trie;
  int wds = dict->load_file("words"); //Boggler::load_dictionary("words");

  if (argc > 1)
    nGrads = atoi(argv[1]);
  if (argc > 2)
    stall = atoi(argv[2]);
  if (argc > 3) {
    stallm = atoi(argv[2]);
    stall = atoi(argv[3]);
  }

  cout << "Loaded " << wds << " words from dictionary." << endl;
  cout << "Will run " << nGrads << " gradients on " << nThreads << " threads" << endl;

  struct thread_arg ta[2];
  pthread_t threads[2];
  for (int i=0; i<nThreads; i++) {
    ta[i].b = new Boggler(dict);
    ta[i].N = i;
    ta[i].nGrads = nGrads;
    ta[i].stall = stall;
    ta[i].stallm = stallm;
  }
  pthread_t usert;
  pthread_create(&usert, NULL, quit_loop, NULL);

  double tRun = millis();
  for (int i=0; i<nThreads; i++) {
    pthread_create(threads+i, NULL, gradient_loop, &ta[i]);
  }
  int totalBoards = 0;
  for (int i=0; i<nThreads; i++) {
    pthread_join(threads[i], NULL);
    totalBoards += ta[i].b->num_boards();
    cout << "Thread " << i << " best board: " << endl;
    cout << ta[i].best.pretty_str();
    cout << ta[i].best.c_str() << " (" << ta[i].maxSc << ")" << endl;
  }
  tRun = millis() - tRun;
  printf("Perf: %d boards in %.3f millis = %.0f bds/sec\n", totalBoards, tRun, 1000.0*totalBoards/tRun);
}

static double millis() {
  struct timeval t;
  gettimeofday(&t, NULL);
  return 1000.0 * t.tv_sec + t.tv_usec / 1000.0;
}
