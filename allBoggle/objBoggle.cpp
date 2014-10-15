#include "objBoggle.h"
#include <iostream>

/* The number of points a word of each length is worth. */
static const int wordScores[17] = { 0, 0, 0, 1, 1, 2, 3, 5, 11, 11,11,11,11,11,11,11,11 };

// Base class for DFS algos -- let's them be more compact
class DFS {
	public:
		inline virtual void push(int x, int y, int prev, int len, Trie* t, int c) {}
		inline virtual void find(int x, int y, int prev, int len, Trie* t, int c) {}
		inline virtual void refind(int x, int y, int prev, int len, Trie* t, int c) {}
		inline virtual void pop() {}
		virtual ~DFS() {} // Not important, just needed to shut up g++
};

// Raw score
struct Scorer : public DFS {
	Scorer() : score(0) {}
	inline void find(int x, int y, int prev, int len, Trie* t, int c) {
		score += wordScores[len];
	}
	int score;
};
int Boggler::board_score(const char* lets) {
	Scorer sc;
	parse_board(lets);
	loop_dfs<Scorer>(sc);
	num_boards_++;
	num_points_ += sc.score;
	return sc.score;
}

// Full tracing
struct Tracer : public DFS {
	Tracer(Trie* t) : level(0), dict(t) {}
	inline void sp() { for (int i=0; i<level; i++) { printf(" "); } }
	
	inline void find(int x, int y, int prev, int len, Trie* t, int c) {
		if (dict->reverse_lookup(t, &out)) {
			sp(); printf("+ found '%s%c'\n", out.c_str(), c+'a');
		}
	}
	inline void refind(int x, int y, int prev, int len, Trie* t, int c) {
		if (dict->reverse_lookup(t, &out)) {
			sp(); printf("- found '%s%c', but was already marked.\n", out.c_str(), c+'a');
		}
	}
	
	inline void push(int x, int y, int prev, int len, Trie* t, int c) {
		if (dict->reverse_lookup(t, &out)) {
			sp(); printf("> (%d,%d) '%s%c'\n", x, y, out.c_str(), c+'a');
		}
		level++;
	}
	inline void pop() { level--; }
	
	int level;
	Trie* dict;
	string out;
};
void Boggler::board_trace(const char* lets) {
	Tracer t(dict);
	parse_board(lets);
	loop_dfs<Tracer>(t);
}

// Construct a vector of all the words on a board
struct Words : public DFS {
	Words(vector<string>* o) : numLets(0), out(o) 
	{ for (int i=0; i<17; i++) lets[i] = '\0'; }
	
	inline void find(int x, int y, int prev, int len, Trie* t, int c) {
		lets[numLets++] = c + 'a';
		out->push_back(lets);
		lets[--numLets] = '\0';
	}
	
	inline void push(int x, int y, int prev, int len, Trie* t, int c) {
		lets[numLets++] = c + 'a';
	}
	inline void pop() {
		lets[--numLets] = '\0';
	}
	
	int numLets;
	char lets[17];
	vector<string>* out;
};
void Boggler::board_words(const char* lets, vector<string>* out) {
 	parse_board(lets);
	Words w(out);
	loop_dfs<Words>(w);
}

// Print out paths to each word
struct Paths : public DFS {
	Paths() : numLets(0) {
		for (int i=0; i<18; i++) lets[i] = '\0';
	}
	
	inline void push(int x, int y, int prev, int len, Trie* t, int c) {
		path.push_back(make_pair(x,y));
		lets[numLets++] = c + 'a';
	}
	inline void pop() {
		path.pop_back();
		lets[--numLets] = '\0';
	}
	
	inline void find(int x, int y, int prev, int len, Trie* t, int c) {
		lets[numLets++] = c + 'a';
		cout << lets;
		for (unsigned i=0; i<path.size(); i++) {
			cout << " " << path[i].second << path[i].first;
		}
		cout << " " << y << x;
		cout << endl;
		lets[--numLets] = '\0';
	}	
	
	int numLets;
	char lets[18];
	vector<pair<int,int> > path;
};
void Boggler::board_paths(const char* lets) {
	parse_board(lets);
	Paths w;
	loop_dfs<Paths>(w);
}

// Check the board for validity and move it into a 2D array
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

Boggler::Boggler(Trie* t) : dict(t), found(t) {}

// Apply an arbitrary algorithm on top of the basic Boggle DFS
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
				if (!found.marked(t,c)) {
					dfs.find(cx, cy, prev, bLen, t, c);
					found.mark(t, c);
				} else {
					dfs.refind(cx, cy, prev, bLen, t, c);
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
template<class BFS>
void Boggler::loop_bfs(BFS& bfs) {
	struct state {
		Trie* t;
		int len;
		int x, y;
		int prev;
	};
	
	// Initial states: start the search from each position
	queue<state> states;
	for (int i=0; i<4; i++) {
		for (int j=0; j<4; j++) {
			if (dict->starts_word(bd[i][j])) {
		   	  	// STL question: how do I add a new state to the end of states
		   	  	// without unnecessary copying?
		   	  	state s;
		   	  	s.t = dict->descend(bd[i][j]);
		   	  	s.len = bd[i][j]==kQ ? 2 : 1;
		   	  	s.x = i; s.y = j;
		   	  	s.prev = cell_mask(i,j);
		   	  	states.push(state);
			}
		}
	}
	
	// Do the BFS
	while (!states.empty()) {
		state st = states.pop();
		int lx = st.x==0 ? 0 : st.x-1; int hx = st.x==3 ? 4 : st.x+2;
		int ly = st.y==0 ? 0 : st.y-1; int hy = st.y==3 ? 4 : st.y+2;
		int prev = st.prev | cell_mask(x,y);
		
		for (int cx=lx; cx<hx; cx++) {
			for (int cy=ly; cy<hy; cy++) {
				// Make sure we haven't been here before
				if (prev & cell_mask(cx,cy)) continue;
				
				int c = bd[cx][cy];
				int bLen = st.len + (c==kQ ? 2 : 1);
				if (bLen >= 3 && st.t->is_word(c)) {
					if (!found.marked(st.t,c)) {
						bfs.find(cx, cy, prev, bLen, st.t, c);
						found.mark(t, c);
					} else {
						bfs.refind(cx, cy, prev, bLen, st.t, c);
					}
				}
				
				// Add fertile paths to the search tree
				if (st.t->starts_word(c)) {
					state s;
					s.t = st.t->descend(c);
					s.len = bLen;
					s.x = cx; s.y = cy;
					s.prev = prev;
					states.push(s);
				}
			}
		}
	}
}
*/
