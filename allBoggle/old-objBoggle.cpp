#include "objBoggle.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const unsigned int MASK_WORD = 1 << 0;
static const unsigned int MASK_Q    = 1 << 1;
static const unsigned int MASK_ADDR = ~(MASK_WORD | MASK_Q);
static const unsigned int MEM_ALIGN = 2;

/* The number of points a word of each length is worth. */
static const int wordScores[17] = { 0, 0, 0, 1, 1, 2, 3, 5, 11, 11,11,11,11,11,11,11,11 };

// Root node of the dictionary shared amongst all bogglers
// I should eventually finagle things so that this is const
static unsigned long start[26];
static unsigned int marksLen=0;
static unsigned int s_low=(unsigned)-1L, s_hi=0;

static inline int mark_idx(int ad)  { return (ad-s_low)>>(5+MEM_ALIGN); }
static inline int mark_mask(int ad) { return 1<<(((ad-s_low)>>MEM_ALIGN)%32); }
static inline int to_cell(int x, int y) { return BD_W * y + x; }
static inline int cell_mask(int x, int y) { return 1 << to_cell(x,y); }

/* Explicit board state */

/* Board-solving continuations */
class BoardContinuation {
	public:
		// Paths leading to this cell that can start a word
		unsigned short prev;
		unsigned long* triePos;
		unsigned int length;
		
 		// Words that have been found w/o using the cell
		unsigned long* found;
		unsigned int score;
		
		// More nasty state I forgot about...
		unsigned long* words[MAX_WORDS];
		unsigned int numWords;
		
		// Print out for debugging
		void display();
		
		// Copy all the data long-hand
		BoardContinuation(const BoardContinuation&);
		void clear();
		
		inline void addWord(unsigned long ad) {
			int idx = mark_idx(ad);
			words[numWords++] = &found[idx];
			found[idx] |= mark_mask(ad);
			score += wordScores[length+1];
		}
		
		void unfind_words(int n) {
			while (n--) {
				*words[--numWords] = 0;
				//unsigned long wd = words[--numWords];
				//*found[words >> 5] &= ~(1 << (words&0x1F));
			}
		}
		
		bool found_word(unsigned long ad) {
			return found[mark_idx(ad)] & mark_mask(ad);
		}
		
		BoardContinuation() : prev(0), length(0), found(NULL), numWords(0) {}
};

void printAllWords(unsigned long* node, unsigned long* found, char* wd, int len) {
	for (int i=0; i<26; i++) {
		unsigned long info = node[i];
		if (info & MASK_WORD) {
			unsigned long ad = (unsigned long)(&node[i]);
			if (mark_idx(ad) >= marksLen) {
				fprintf(stderr, "blah!!!\n");
				exit(1);
			}
			if (found[mark_idx(ad)] & mark_mask(ad)) {
				printf("%s%c\n", wd, 'a'+i);
			}
		}
		if (info & MASK_ADDR) {
			wd[len] = 'a' + i;
			wd[len+1] = '\0';
			unsigned long* n = (unsigned long*)(info & MASK_ADDR);
			printAllWords(n, found, wd, len+1);
			wd[len] = '\0';
		}
	}
}

void BoardContinuation::display() {
	printf("Previous cells:\n");
	for (int y=0; y<BD_H; y++) {
		for (int x=0; x<BD_W; x++) {
			printf("%c", (prev & cell_mask(x,y))? 'x' : '-');
		}
		printf("\n");
	}

	printf("Found %d points in %d words\n", score, numWords);
	
	// Dictionary addresses are not easiliy reversible.
	// Printing out a list of words would require iterating over the whole dictionary
	char buf[18] = "";
	//printAllWords(start, found, buf, 0);
	printf("done, start=%08X\n", (unsigned long)start);
}

// Create a new continuation based on an old one
BoardContinuation::BoardContinuation(const BoardContinuation& b) {
	prev = b.prev;
	triePos = b.triePos;
	found = (unsigned long*)malloc(marksLen);
	memcpy(found, b.found, marksLen);
	
	// Don't copy over previously found words
	// This is an optimization
	numWords = 0;
	length = 0;
}

void BoardContinuation::clear() {
	prev = 0;
	triePos = NULL;
	length = 0;
	if (found) {
		unfind_words(numWords);
	} else {
		found = (unsigned long*)calloc(marksLen, 1);
		numWords = 0;
	}
}
/* --------------------- */

/* Board-solving */
/* Given a position on the board, the word as seen so far, and a pointer
 *  to the node in the dictionary that represents the word so far, iterate
 *  out one block in each direction. If the sequence of letters formed by
 *  adding that block is a word, score it, and mark it as found. If it begins
 *  a word, continue the search starting from the that block.
 *
 * This is essentially an exhaustive depth-first search on the board, except
 *  that the restriction that wdSoFar must be the start of some word dramatically
 *  reduces the number of paths that must be traversed.
 */
#ifdef TRACING
static char sofar[18] = "";
#endif
int Boggler::solve(int x, int y)
{
  int cx,cy;
  int dx,dy;

  for (dx = -1; dx <= 1; dx++) {
    cx = x + dx;
    if (cx < 0 || cx > 3) continue;
    for (dy = -1; dy <= 1; dy++) {
      cy = y + dy;
      if (cy < 0 || cy > 3) continue;
      if (state->prev & cell_mask(cx,cy)) continue;

      // Add this block to the word and see what we get.
      unsigned long info = state->triePos[bd[cx][cy]];
      unsigned long ad = (unsigned long)(&state->triePos[bd[cx][cy]]);

      // Check if it's a word, and score it if so.
      if (state->length >= 1 &&  (info & MASK_WORD)) {
		//if (!( marks[mark_idx(ad)] & mark_mask(ad)))
		if (!state->found_word(ad))
		{
		  // The number of letters is n+1 + #of q's
		  //state->score += wordScores[state->length+1 + ((info & MASK_Q)/MASK_Q)];
		  
		  state->length += 1 + ((info & MASK_Q)/MASK_Q);
		  state->addWord(ad);
	
		  // Mark this word as "already found" and save the address for cleanup
		  //marks[mark_idx(ad)] |= mark_mask(ad);
		  //words[numWords++] = &marks[mark_idx(ad)];
		}
      }

      // If this letter sequence starts other words, continue the search.
      if (info & MASK_ADDR) {
      	unsigned long* tmp = state->triePos;
		state->prev |= cell_mask(cx, cy);
		state->triePos = (unsigned long*)(info & MASK_ADDR);
		solve(cx, cy);
		state->triePos = tmp;
		state->prev &= ~cell_mask(cx, cy);
      }
    }
  }

  return state->score;
}

/* Start the solving recurrence on each block on the board. */
int Boggler::loopOverBoard() {
  //int score = 0;

  // We've yet to visit any block, so clear out this array
  //prev = 0;
  state->score = 0;
  state->prev = 0;

  // Start the recursion at each block on the board.
  for (int i=0; i<4; i++) {
    for (int j=0; j<4; j++) {
      state->prev = cell_mask(i,j); // mark the first cell as visited
	
	  printf("%d %d:\n", i, j);
      unsigned long info = start[bd[i][j]];
	  state->display();

#ifdef TRACING
      sofar[0] = 'a' + bd[i][j];
#endif
      info = start[bd[i][j]];
      state->triePos = (unsigned long*)(info & MASK_ADDR);
      state->length = 1;
      solve(i,j);

      state->prev = 0;
    }
  }

  // Clear "found" marks
  /*
  unsigned long* tmp;
  while (numWords) {
    numWords--;
    tmp = words[numWords];
    *tmp = 0;
  }
  */
  int score = state->score;
  state->display();
  state->clear();

  return score;
}

// External interface
int Boggler::board_score(const char* lets) {
  // Sanity check
  if (strlen(lets) != 16) { printf("len = %d\n", (int)strlen(lets)); exit(1); }
  for (int i=0; i<16; i++) {
    if (lets[i] < 'a' || lets[i] > 'z') {
      printf("board: '%s'\n", lets);
      exit(1);
    }
  }

  // Copy over to a 2D array
  for (int i=0; i<16; i++)
    bd[i/4][i%4] = lets[i]-'a';
  int score = loopOverBoard();
  numBoards_++;
  numPoints_ += score;
  return score;
}

/* Instantiation */
Boggler::Boggler() {
  if (marksLen == 0) {
  	fprintf(stderr, "Must load a dictionary before instantiating a boggler\n");
  	return;
  }
  //marks = (unsigned long*)calloc(marksLen, 1);
  state = new BoardContinuation();
  state->clear();

  numBoards_ = 0;
  //prev = 0;
  for (int i=0; i<4; i++) for (int j=0; j<4; j++) bd[i][j] = '0';
}

/* Dictionary loading */
// Do a DFS on this trie to accumulate some statistics
// Used to determine how large of a bit vector is necessary for each Boggler
static unsigned int s_mask=0;
static void loop_stats(unsigned long* addr) {
  for (int i=0; i<26; i++) {
    unsigned long node = addr[i];
    unsigned long ad = node & MASK_ADDR;
    if (ad) {
      s_low = s_low > ad ? ad : s_low;
      s_hi = s_hi < ad ? ad : s_hi;
      s_mask |= ad;
      loop_stats((unsigned long*)ad);
    }
  }
}

/*
 * Hi-speed dictionary.
 * Allows word lookup from any particular starting
 * location (location being a sequence of letters)
 * Supports two operations:
 *   1. isWord
 *   2. startsWord
 *
 * Designed to be used in Boggle, where those are the
 * only two operations that matter.
 */
static int amountAlloced=0;
static void initNodes() {
  for (int i=0;i<26;i++)
    start[i] = 0;
}

/* Adds a new word to the existing dictionary Trie. New nodes are allocated
 * as necessary, and are annotated with information about whether they are full
 * words or contain a 'q'.
 */
static void addWord(char *wd) {
  unsigned long* cur = start;
  int pos,i,numQ=0;
  char c;

  /* Descend the trie until we reach the end of the word
   * Create new nodes as necessary.
   * As they are created, nodes are marked with the number of Q's
   * that have occurred thus far in the word.
   */
  for (pos=0; (c=wd[pos]); pos++)
  {
    if (c=='q') numQ++;

    if (!wd[pos+1]) {
      // End of the word.
      // Mark this node as a complete word.
      cur[c-'a'] = cur[c-'a'] | MASK_WORD;

      // If it contains a 'q', note that.
      if (numQ) cur[c-'a'] = cur[c-'a'] | MASK_Q;
      break;
    }
    else if (!(cur[c-'a'] & MASK_ADDR))
    {
      // Need to malloc a new node to continue down the tree
      unsigned long save = cur[c-'a'] & MASK_WORD; // save isWord status
      cur[c-'a'] = (unsigned long)malloc(26*sizeof(unsigned long));
      cur[c-'a'] = cur[c-'a'] + save;
      if (numQ)
	cur[c-'a'] = cur[c-'a'] | MASK_Q;
      amountAlloced += 26*sizeof(unsigned long);

      cur = (unsigned long*)(cur[c-'a'] & MASK_ADDR);
      for (i=0;i<26;i++)
	cur[i] = 0;
    }
    else
    {
      cur = (unsigned long*)(cur[c-'a'] & MASK_ADDR);
    }

    // Treat 'qu' as 'q'
    if (wd[pos] == 'q' && wd[pos+1] == 'u')
      pos++;
  }
}

/* Form a Trie from all words in a dictionary file. */
static int loadAllWords(char* dict) {
  char wd[256];
  int numWords = 0;

  FILE* f = fopen(dict,"r");
  if (!f) {
    fprintf(stderr, "Couldn't open dictionary file '%s'\n", dict);
    return -1;
  }

  initNodes();

  while (!feof(f) && fscanf(f,"%s",wd))
  {
    // Screen for valid Boggle words.
    // These consist only of lowercase letters.
    // All q's must be immediately followed by u's
    int bad = 0;
    for (int i=0; wd[i]; i++) {
      if (wd[i]<'a' || wd[i]>'z') bad=1;
      if (wd[i]=='q' && wd[i+1]!='u') bad=1;
    }
    if (bad) continue;

    addWord(wd);
    numWords++;
  }
  fclose(f);

  return numWords;
}

/* External interface */
int Boggler::load_dictionary(char* dict) {
  if (marksLen != 0) return -1;

  int numWords = loadAllWords(dict);
  if (numWords <= 0) return numWords;

  loop_stats(start);
  /*printf("Stats:\n");
    printf("Low:  0x%08x = %u\n", s_low, s_low);
    printf("High: 0x%08x = %u\n", s_hi, s_hi);
    printf("mask: 0x%08x\n", s_mask);
   */
  marksLen = (s_hi-s_low)/(8 * (1<<MEM_ALIGN));
  //printf("Marks buffer: %d bytes\n", (int)marksLen);

  return numWords;
}
