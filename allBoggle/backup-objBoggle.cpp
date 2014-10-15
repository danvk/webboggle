#include "objBoggle.h"

/* The number of points a word of each length is worth. */
static const int wordScores[17] = { 0, 0, 0, 1, 1, 2, 3, 5, 11, 11,11,11,11,11,11,11,11 };

static inline int to_cell(int x, int y) { return (y<<2) + x; }
static inline int cell_mask(int x, int y) { return 1 << to_cell(x,y); }

static const int kQ = 'q' - 'a';

template<class DFS>
void Boggler::apply_dfs(int x, int y, int prev, int len, Trie* t, DFS& dfs) {
	int lx = x==0 ? 0 : x-1; int hx = x==3 ? 4 : x+2;
	int ly = y==0 ? 0 : y-1; int hy = y==3 ? 4 : y+2;
	prev |= cell_mask(x,y);
	
	for (int cx=lx; cx<hx; cx++) {
		for (int cy=ly; cy<hy; cy++) {
			// Make sure we haven't been here before
			if (prev & cell_mask(cx,cy)) continue;
			
			int c = bd[cx][cy];
			int bLen = len + (c==kQ ? 2 : 1);
			if (bLen >= 3 && t->is_word(c)) {
				// This foundmark stuff might be more appropriate in the DFS
				if (!found.marked(t,c)) {
					dfs.find(cx, cy, prev, bLen, t, c);
					found.mark(t, c);
				}
			}
			if (t->starts_word(c)) {
				dfs.push(cx, cy, prev, bLen, t, c);
				apply_dfs<DFS>(cx, cy, prev, bLen, t->descend(c), dfs);
				dfs.pop();
			}
		}
	}
	
	prev &= ~cell_mask(x,y);
}

template<class DFS>
void Boggler::loop_dfs(DFS& dfs) {
  for (int i=0; i<4; i++) {
    for (int j=0; j<4; j++) {
   	  if (dict->starts_word(bd[i][j])) {
   	  	int bLen = bd[i][j]==kQ ? 2 : 1;
   	  	dfs.push(i, j, 0, bLen, dict, bd[i][j]);
   	  	apply_dfs<DFS>(i, j, 0, bLen, dict->descend(bd[i][j]), dfs);
   	  	dfs.pop();
   	  }
   	}
  }
  found.unmark();
}

/*
DFS {
	void push(int x, int y, int prev, int len, Trie* t, int c);
	void find(int x, int y, int prev, int len, Trie* t, int c);
	void pop();
};
*/
struct Scorer {
	public:
		Scorer() : score(0) {}
		inline void find(int x, int y, int prev, int len, Trie* t, int c) {
			score += wordScores[len];
		}
		
		inline void push(int x, int y, int prev, int len, Trie* t, int c) {}
		inline void pop() {}
		
		int score;
};
int Boggler::solve_with_dfs(const char* lets) {
	struct Scorer sc;
	parse_board(lets);
	loop_dfs<struct Scorer>(sc);
	return sc.score;
}

/* Score a board */
int Boggler::solve(int x, int y, Trie* t, int len) {
	int lx = x==0 ? 0 : x-1; int hx = x==3 ? 4 : x+2;
	int ly = y==0 ? 0 : y-1; int hy = y==3 ? 4 : y+2;
	int score = 0;
	prev |= cell_mask(x,y);
	
	for (int cx=lx; cx<hx; cx++) {
		for (int cy=ly; cy<hy; cy++) {
			// Make sure we haven't been here before
			if (prev & cell_mask(cx,cy)) continue;
			
			int c = bd[cx][cy];
			int bLen = len + (c==kQ ? 2 : 1);
			if (bLen >= 3 && t->is_word(c)) {
				if (!found.marked(t,c)) {
					num_words_++;
					score += wordScores[bLen];
					found.mark(t, c);
				}
			}
			if (t->starts_word(c)) {
				score += solve(cx, cy, t->descend(c), len + (c==kQ ? 2 : 1));
			}
		}
	}
	
	prev &= ~cell_mask(x,y);
	return score;
}

int Boggler::board_score(const char* lets) {
  parse_board(lets);
  int score = 0;
  for (int i=0; i<4; i++)
    for (int j=0; j<4; j++)
   	  if (dict->starts_word(bd[i][j]))
   	  	score += solve(i, j, dict->descend(bd[i][j]), bd[i][j]==kQ ? 2 : 1);
  
  found.unmark();
  num_boards_++;
  num_points_ += score;
  return score;
}

/* Do the same as above, but with tracing */
void Boggler::solve_trace(int x, int y, Trie* t, int len, const string& sofar) {
	int lx = x==0 ? 0 : x-1; int hx = x==3 ? 4 : x+2;
	int ly = y==0 ? 0 : y-1; int hy = y==3 ? 4 : y+2;
	int score = 0;
	prev |= cell_mask(x,y);
	
	for (int cx=lx; cx<hx; cx++) {
		for (int cy=ly; cy<hy; cy++) {
			// Make sure we haven't been here before
			if (prev & cell_mask(cx,cy)) continue;
			
			int c = bd[cx][cy];
			int bLen = len + (c==kQ ? 2 : 1);
			if (len >= 2 && t->is_word(c)) {
				if (!found.marked(t,c)) {
					num_words_++;
					score += wordScores[bLen];
					found.mark(t, c);
					printf("%3d. Found word '%s%c', len=%d, scoring %d\n",
							(int)num_words_, sofar.c_str(), c+'a', bLen, wordScores[bLen]);
				} else {
					printf("Found '%s%c' but was already marked\n", sofar.c_str(), c+'a');
				}
			}
			if (t->starts_word(c)) {
				solve_trace(cx, cy, t->descend(c), bLen, sofar + string(1,c+'a'));
			}
		}
	}
	
	prev &= ~cell_mask(x,y);
}

void Boggler::board_trace(const char* lets) {
  parse_board(lets);
  for (int i=0; i<4; i++)
    for (int j=0; j<4; j++)
   	  if (dict->starts_word(bd[i][j]))
   	  	solve_trace(i, j, dict->descend(bd[i][j]), bd[i][j]==kQ ? 2 : 1, string(1,'a'+bd[i][j]));
  
  found.unmark();
}

/* Find all words on a board */
void Boggler::loop_explicit(vector<string>* out) {
  // Start the recursion at each block on the board.
  for (int i=0; i<4; i++) {
    for (int j=0; j<4; j++) {
   	  if (dict->starts_word(bd[i][j])) {
        string sofar(1,'a'+bd[i][j]);
   	  	solve_explicit(i, j, dict->descend(bd[i][j]), sofar, out);
   	  }
    }
  }
}

void Boggler::solve_explicit(int x, int y, Trie* t, const string& sofar, vector<string>* out) {
	int lx = x==0 ? 0 : x-1; int hx = x==3 ? 4 : x+2;
	int ly = y==0 ? 0 : y-1; int hy = y==3 ? 4 : y+2;
	
	prev |= cell_mask(x,y);
	for (int cx=lx; cx<hx; cx++) {
		for (int cy=ly; cy<hy; cy++) {
			// Make sure we haven't been here before
			if (prev & cell_mask(cx,cy)) continue;
			
			int c = bd[cx][cy];
			string tmp = sofar + string(1, c+'a');
			int bogLen = tmp.length() + (tmp.find("q") == string::npos ? 0 : 1);
			if (bogLen >= 3 && t->is_word(c)) {
				if (!found.marked(t,c)) {
					out->push_back(tmp);
					found.mark(t, c);
				}
			}
			if (t->starts_word(c)) {
				solve_explicit(cx, cy, t->descend(c), tmp, out);
			}
		}
	}
	prev &= ~cell_mask(x,y);
}

void Boggler::board_words(const char* lets, vector<string>* out) {
  parse_board(lets);
  loop_explicit(out);
  found.unmark();
  num_boards_++;
}
/********************/

bool Boggler::parse_board(const char* lets) {
  // Sanity check
  if (strlen(lets) != 16) { return false; }
  for (int i=0; i<16; i++) {
    if (lets[i] < 'a' || lets[i] > 'z') {
      return false;
    }
  }

  // Copy over to a 2D array
  for (int i=0; i<16; i++)
    bd[i/4][i%4] = lets[i]-'a';
  return true;
}

Boggler::Boggler(Trie* t) : dict(t), found(t) {
}