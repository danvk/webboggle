/*********
 * This program solves Boggle boards. Boggle is a board game produced by
 * Milton Bradley. For info on the rules, see
 *     http://en.wikipedia.org/wiki/Boggle
 * 
 * The reason I like this code is because the data structure perfectly mirrors
 *   the problem. I had never heard of a Trie before I wrote this, but as I
 *   thought about the problem, I developed the idea of a Trie on my own. This
 *   code is incredibly fast -- I've seen it solve upwards of 6000 boards per
 *   second on a 1.5 GHz Pentium. This speed is essential if you want to answer
 *   questions like "What is the highest-scoring boggle board?" This was the
 *   question that motivated this code. I wrote a genetic algorithm to answer
 *   this question. The best I came up with can be found at:
 *
 *     http://www.owlnet.rice.edu/~danvk/catdlinemaropets.html
 *
 * The code isn't too short, but essentially everything but two routines,
 *   solve and addWord, is glue.
 * 
 * To compile and run on my Mac, I run:
 *    gcc -O3 -std=c99 -o boggle googleboggle.c
 *
 * A sample session with this program might look like this:
 *
 *    danvk% ./boggle 
 *    abcdefghijklmnop
 *    25
 *    catdlinemaropets
 *    2724
 *    <Ctrl-D>
 *
 * The input is the blocks on a Boggle board, and the output is the maximum
 * score that can be achieved on that Boggle board.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* The dictionary, implemented as a Trie. Each node stores 26 addresses, which
 * are pointers to other nodes represented by adding an 'a'..'z' to the end of
 * the word represented by the current node. Since malloc returns double-word
 * aligned addresses, extra information can be packed into the lower 3 bits.
 */
#define MASK_ADDR 0xFFFFFFF8
#define MASK_WORD 0x00000001
#define MASK_USED 0x00000002
#define MASK_Q	  0x00000004
static unsigned long start[26];

static void loadAllWords(char* dict);

/* Since we mark all discovered words, save a list of them to unmark later. */
static unsigned long* wordMarks[2048];
static unsigned int numWords = 0;

static int bd[4][4];   /* The characters on the board. */
static int prev[4][4]; /* Whether a block has been visited yet. */

/* The number of points a word of each length is worth. */
static int wordScores[17] = { 0, 0, 0, 1, 1, 2, 3, 5, 11, 11,11,11,11,11,11,11,11 };
static int score = 0;

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
static void solve(int x, int y, int n, unsigned long* node)
{
	int cx,cy;
	int dx,dy;
	for (dx = -1; dx <= 1; dx++) {
		cx = x + dx;
		if (cx < 0 || cx > 3) continue;
		for (dy = -1; dy <= 1; dy++) {
			cy = y + dy;
			if (cy < 0 || cy > 3) continue;
			if (prev[cx][cy]) continue;
			
			// Add this block to the word and see what we get.
			unsigned long info = node[bd[cx][cy]];
			
			// Check if it's a word, and score it if so.
			if (n >= 1 &&  (info & MASK_WORD)
			           && !(info & MASK_USED))
			{
				// The number of letters is n+1 + #of q's
				score += wordScores[n+1 + ((info & MASK_Q)>>2)];

				// Mark this word as "already found"
				node[bd[cx][cy]] = info ^ MASK_USED;
				wordMarks[numWords++] = &node[bd[cx][cy]];
			}
			
			// If this letter sequence starts other words, continue the search.
			if (info & MASK_ADDR) {
				prev[cx][cy] = n;
				solve(cx, cy, n+1, (unsigned long*)(info & MASK_ADDR));
				prev[cx][cy] = 0;
			}
		}
	}
}

/* Start the solving recurrence on each block on the board. */
static void loopOverBoard() {
	int wdSoFar[18];
	score = 0;
	
	// We've yet to visit any block, so clear out this array
	for (int i=0;i<4;i++)
		for (int j=0;j<4;j++)
			prev[i][j] = 0;
	
	// Start the recursion at each block on the board.
	for (int i=0; i<4; i++) {
		for (int j=0; j<4; j++) {
			prev[i][j] = 1; // mark the first cell as visited

			unsigned long info = start[bd[i][j]];			
			solve(i,j,1, (unsigned long*)(info & MASK_ADDR));
			
			prev[i][j] = 0;
		}
	}

	// Clear "found" marks
	unsigned long tmp;
	while (numWords) {
		numWords--;
		tmp = *wordMarks[numWords];
		*wordMarks[numWords] = tmp ^ MASK_USED;
	}
}

void fastBoggle(char* dict) {

}

/* This program expects a dictionary file containing one word on each line.
 */
static char defDict[] = "/usr/share/dict/words";
int main(int argc, char** argv) {
	char* dict;
	char twd[256];
	
	if (argc<2) dict = defDict;
	else        dict = argv[1];
		
	loadAllWords(dict);	
	
	/* Read in a board from stdin and score it, putting the result on stdout.
	 * The "qu" block is referred to as "q". An example input:
	 *   catdlinemaropets
	 * With the standard web2a dictionary, this scores a 2724.
	 */
	while (scanf("%s", twd) != -1) {
		char wd[18];
		int i, j=0, len;
		for (i=0; twd[i]; i++) {
			wd[j++] = twd[i];

			// Stop scanning if the board is too long.
			if (j==17 && twd[i+1]) {
				printf("Invalid board: %s\n", twd);
				break;
			}
		}
		wd[j] = '\0';

		// Valid boards have 4x4=16 characters on them
		if (strlen(wd) != 16) {
			printf("Invalid board: %s\n", wd);
			continue;
		}

		for (int i=0; wd[i]; i++)
			bd[i/4][i%4] = wd[i]-'a';

		loopOverBoard();
		printf("%d\n", score);
		fflush(stdout);
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
static void loadAllWords(char* dict) {
	char wd[256];

	FILE* f = fopen(dict,"r");
	if (!f) {
		fprintf(stderr, "Couldn't open dictionary: %s\n", dict);
		exit(1);
	}
	
	initNodes();
	
	while (!feof(f) && fscanf(f,"%s",wd))
	{
		// Screen for valid Boggle words. These consist only of lowercase letters.
		if (wd[0]<'a' || wd[0]>'z') continue;

		addWord(wd);
	}
	
	fclose(f);
}
