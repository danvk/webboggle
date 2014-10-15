#include "partialBoggle.h"

static inline int to_cell(int x, int y) { return (y<<2) + x; }
static inline int cell_mask(int x, int y) { return 1 << to_cell(x,y); }
static const int wordScores[17] = { 0, 0, 0, 1, 1, 2, 3, 5, 11, 11,11,11,11,11,11,11,11 };
static const int kQ = 'q'-'a';

// Pre-compute everything about the board
bool PartialBoggler::partial_solve(const char* lets, PartialBoard* out) {
	out->clear();
	if (!parse_board(lets))
		return false;
	
	// Prepare the FoundMarks
	for (int i=0; i<16; i++) {
		if (out->found[i])
			out->found[i]->unmark();
		else
			out->found[i] = new FoundMark(dict);
	}
	
	for (int i=0; i<4; i++) {
		for (int j=0; j<4; j++) {
			int c = bd[i][j];
			
			// A word can always start on any cell
			Path p;
			p.path = 0;
			p.t = dict;
			p.len = 0;
			out->paths[to_cell(i,j)].push_back(p);
			
			if (dict->starts_word(c)) {
				solve(i, j, dict->descend(c), 1, out);
			}
		}
	}
	return true;
}

/*
- When a word is found, score it and mark it in the FoundMarks of all unused cells
- Find words regardless of whether they've been found before
- At the start of each solve call, save the prev/trie position as a path for this cell
*/
void PartialBoggler::solve(int x, int y, Trie* t, int len, PartialBoard* out) {
	// Save the path to this cell
	prev |= cell_mask(x,y);
	
	int lx = x==0 ? 0 : x-1; int hx = x==3 ? 4 : x+2;
	int ly = y==0 ? 0 : y-1; int hy = y==3 ? 4 : y+2;
	
	for (int cx=lx; cx<hx; cx++) {
		for (int cy=ly; cy<hy; cy++) {
			// Make sure we haven't been here before
			if (prev & cell_mask(cx,cy)) continue;
			
			// This feels wasteful -- I should be able to insert directly into paths...
			Path p;
			p.path = prev;
			p.t = t;
			p.len = len;
			out->paths[to_cell(cx,cy)].push_back(p);
			
			int c = bd[cx][cy];
			int bLen = len + (c==kQ ? 2 : 1);
			if (bLen >= 3 && t->is_word(c)) {
				// Mark this word in all cells that were not used in forming it
				int tmpPrev = prev | cell_mask(cx,cy);
				int ws = wordScores[bLen];
				for (int cell=0; cell<16; cell++) {
					if (!(tmpPrev & 0x1)) {
						out->score[cell] += ws;
						out->found[cell]->mark(t, c);
					}
					tmpPrev >>= 1;
				}
			}
			if (t->starts_word(c)) {
				solve(cx, cy, t->descend(c), len + (c==kQ ? 2 : 1), out);
			}
		}
	}
	
	prev &= ~cell_mask(x,y);
}

static void print_word(Trie* dict, Trie* t, char c) {
	string wd;
	if (dict->reverse_lookup(t, &wd)) {
		printf(" '%s%c'\n", wd.c_str(), c);
	}
}

/* Given the precomputations, finish the work. Need a way to ensure
 * that the correct board letters are in the bd array already. */
int PartialBoggler::solve(const PartialBoard& pbd, int pos, char let) {
	FoundMark* fm = pbd.found[pos]; // words that don't go through pos
	int score = pbd.score[pos];        // score of above words
	vector<Path>::const_iterator path = pbd.paths[pos].begin();
	vector<Path>::const_iterator end  = pbd.paths[pos].end();
	fm->set_waterline();           // we only want to clear newly found words later
	int iLet = let - 'a';
	int cx = pos % 4, cy = pos>>2;
	int extraLen = (iLet == kQ ? 2 : 1);
/*
	printf("changed (%d, %d) to '%c'\n", cx, cy, let);
	printf("Solving ");
	int tmp = bd[pos%4][pos/4];
	bd[pos%4][pos/4] = iLet;
	for (int i=0; i<16; i++) {
		printf("%c", 'a'+bd[i%4][i/4]);
	}
	printf("\n");
	bd[pos%4][pos/4] = tmp;
	printf("Initial score: %d\n", score);
	printf("Previously found:\n");
	fm->print_words(dict);
	//printf("Paths to explore: %d\n", pbd.paths[pos].size());
//*/
	for (; path != end; ++path) {
		Trie* base = path->t;
		if (base->starts_word(iLet)) {
			score += solve(cx, cy, base->descend(iLet), path->len + extraLen, path->path, fm);
		}
		if (base->is_word(iLet) && !fm->marked(base, iLet)) {
			int bLen = path->len + extraLen;
			score += wordScores[bLen];
			fm->mark(base, iLet);
			//print_word(dict, base, let);
		}
	}
	fm->unmark_waterline();
	return score;
}

int PartialBoggler::solve(int x, int y, Trie* t, int len, unsigned int prev, FoundMark* fm) {
	int lx = x==0 ? 0 : x-1; int hx = x==3 ? 4 : x+2;
	int ly = y==0 ? 0 : y-1; int hy = y==3 ? 4 : y+2;
	int score = 0;
	prev |= cell_mask(x,y);
	//printf("exploring ");
	//print_word(dict, t, '?');
	
	for (int cx=lx; cx<hx; cx++) {
		for (int cy=ly; cy<hy; cy++) {
			// Make sure we haven't been here before
			if (prev & cell_mask(cx,cy)) continue;
			
			int c = bd[cx][cy];
			int bLen = len + (c==kQ ? 2 : 1);
			//printf(" checking ");
			//print_word(dict, t, 'a'+c);
			if (bLen >= 3 && t->is_word(c)) {
				if (!fm->marked(t,c)) {
					score += wordScores[bLen];
					fm->mark(t, c);
					//print_word(dict, t, c+'a');
				}
			}
			if (t->starts_word(c)) {
				score += solve(cx, cy, t->descend(c), len + (c==kQ ? 2 : 1), prev, fm);
			}
		}
	}
	
	prev &= ~cell_mask(x,y);
	return score;
}

bool PartialBoggler::parse_board(const char* lets) {
  // Sanity check
  if (strlen(lets) != 16) { return false; }
  for (int i=0; i<16; i++) {
    if (lets[i] < 'a' || lets[i] > 'z') {
      return false;
    }
  }

  // Copy over to a 2D array
  for (int i=0; i<16; i++)
    bd[i%4][i/4] = lets[i]-'a';
  return true;
}

/* Partial board implementation */
PartialBoard::PartialBoard() {
	for (int i=0; i<16; i++) {
		found[i] = NULL;
		score[i] = 0;
	}
}

void PartialBoard::clear() {
	for (int i=0; i<16; i++) {
		if (found[i])
			found[i]->unmark();
		score[i] = 0;
		paths[i].clear();
	}
}

void PartialBoard::print_debug(Trie* base, int i) {
	if (i == -1) {
		for (int idx=0; idx<16; idx++) {
			print_debug(base, idx);
		}
	} else {
		printf("cell %d\n", i);
		printf("  score: %d\n", score[i]);
		printf("  paths: %d\n", (int)paths[i].size());
		for (unsigned j=0; j<paths[i].size(); j++) {
			string tmp;
			printf("  %2d. %04X (len=%d)\n", j, paths[i][j].path, paths[i][j].len);
			if (base->reverse_lookup(paths[i][j].t, &tmp)) {
				printf("       %s\n", tmp.c_str());
			}
		}
	}
}
