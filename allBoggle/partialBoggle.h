#ifndef PARTIAL_BOGGLE
#define PARTIAL_BOGGLE

// Punch out each individual die of a boggle board and solve as much
// as possible w/o knowing how that die will fall.

#include "trie.h"
//#include "found_mark.h"
#include "found_mark_debug.h"
#include <vector>
using namespace std;

typedef struct {
	unsigned int path;
	Trie* t;
	unsigned int len;
} Path;

// A board with all completions partially pre-computed
class PartialBoard {
	public:
		PartialBoard();
		void clear();
		void print_debug(Trie* base, int i=-1);

	private:
		FoundMark* found[16];
		int score[16];
		
		// State upon arriving at a cell: previous cells, dictionary position
		vector<Path> paths[16];
		
	friend class PartialBoggler;
};

class PartialBoggler {
	public:
		PartialBoggler(Trie* t) : dict(t) {}
		
		// Pre-compute everything about the board
		bool partial_solve(const char* lets, PartialBoard* out);
		
		// Plug in one change and solve quickly
		int solve(const PartialBoard& bd, int pos, char let);
		bool parse_board(const char* lets);
		
	private:
		Trie* dict;
		int bd[4][4];
		int prev;
		
		void solve(int x, int y, Trie* t, int len, PartialBoard* out);
		int  solve(int x, int y, Trie* t, int len, unsigned int prev, FoundMark* fm);
		
};

#endif