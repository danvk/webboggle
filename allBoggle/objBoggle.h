#ifndef OBJ_BOGGLE_H
#define OBJ_BOGGLE_H

#define BD_W 4
#define BD_H 4
#define CELLS 16

#include <vector>
#include <string>
using namespace std;

#include "trie.h"
#include "found_mark.h"
//#include "found_mark_hash.h"
//#include "found_mark_intrie.h"
//#include "found_mark_judy.h"

class Boggler {
	public:
		// Score a board
		void board_words(const char* bd, vector<string>* out);
		int board_score(const char* bd);
		void board_trace(const char* bd);
		void board_paths(const char* bd);
		
		// Statistics
		inline int num_boards() { return num_boards_; }
		inline int num_points() { return num_points_; }
		inline int num_words()  { return num_words_;  }
		
		Boggler(Trie* t);

	private:
		Trie* dict;
		FoundMark found;
		
		unsigned long num_boards_;
		unsigned long num_points_;
		unsigned long num_words_;
		
		char bd[BD_W][BD_H];
		unsigned int prev;
		
		// disallow
		Boggler(const Boggler&) : found(NULL) {}
		
		// internal use -- does C++ have a way to hide these?
		void solve_explicit(int x, int y, Trie* t, const string& sofar, vector<string>*);
		void loop_explicit(vector<string>*);
		int solve(int x, int y, Trie* t, int len);
		void solve_trace(int x, int y, Trie* t, int len, const string& sofar);
		
		bool parse_board(const char* bd);
		
		// Internals to avoid code duplication
		template<class DFS>
		void loop_dfs(DFS& dfs);
		
		template<class DFS>
		void apply_dfs(int x, int y, int prev, int len, Trie* t, DFS& dfs);
		
		//template<class BFS>
		//void loop_bfs(BFS& bfs);
};

#endif
