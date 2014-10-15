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
static int wordScores[17] = { 0, 0, 0, 1, 1, 2, 3, 5, 11,
							  11,11,11,11,11,11,11,11 };
static int score = 0;

static void scoreWord(const int* wd) {
	int numLets = 0;
	int i; int hasQ = 0;
	for (i=0; wd[i]>=0; i++) {
		//printf("%c", 'a' + wd[i]);
		if (wd[i] ==  ('q'-'a'))
			numLets+=2, hasQ=1;
		else
			numLets++;
	}
	
	/*
	if (showingWords && numLets+hasQ >= 3) {
		//actualWords++;
		//printf("%3d. ", actualWords);
		for (i=0; wd[i]>=0; i++) {
			if (wd[i] == ('q'-'a'))
				putchar('q'), putchar('u');
			else
				putchar('a'+wd[i]);
		}
		printf(" (%d)\n", wordScores[numLets]);
	}
	*/
	
	score += wordScores[numLets];
}

/* Given a position on the board, the word as seen so far, and a pointer
 * to the node in the dictionary that represents the word so far, iterate
 * out one block in each direction.
 */
static void solve(int x, int y, int n, int* wdSoFar, unsigned long* node)
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
			
			// Add the current block to the word
			wdSoFar[n] = bd[cx][cy];
			wdSoFar[n+1] = -1;
			
			unsigned long info = node[bd[cx][cy]];
			
			// Check if it's a word, and score it if so.
			if (n >= 1 &&  (info & MASK_WORD)
			           && !(info & MASK_USED))
			{
				scoreWord(wdSoFar);
				node[bd[cx][cy]] = info ^ MASK_USED;
				wordMarks[numWords++] = &node[bd[cx][cy]];
			}
			
			// Recur, starting with the new word.
			if (info & MASK_ADDR) {
				prev[cx][cy] = n;
				solve(cx, cy, n+1, wdSoFar, (unsigned long*)(info & MASK_ADDR));
				prev[cx][cy] = 0;
			}
		}
	}
}

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
			prev[i][j] = 1;
			
			wdSoFar[0] = bd[i][j];
			wdSoFar[1] = -1;
			
			unsigned long info = start[bd[i][j]];
			
			solve(i,j,1,wdSoFar, (unsigned long*)(info & MASK_ADDR));
			
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
	char twd[256];
	
	loadAllWords(dict);	
	
	/* Read in a board from stdin and score it, putting the result on stdout.
	 * The "qu" block is referred to as "q". An example input:
	 * abcdefghijklmnop
	 */
	while (scanf("%s", twd) != -1) {
		char wd[18];
		int i, j=0, len;
		for (i=0; twd[i]; i++) {
			wd[j++] = twd[i];
	
			if (j==17 && twd[i+1]) {
				printf("Invalid board: %s\n", twd);
				break;
			}
		}
		wd[j] = '\0';

		if (strlen(wd) != 16) {
			printf("Invalid board: %s\n", wd);
			continue;
		}
		
		for (int i=0; wd[i]; i++)
			bd[i/4][i%4] = wd[i+showingWords]-'a';
		
		loopOverBoard();
		printf("%d\n", score);
	}
}

/* High-speed Dictionary */
/*
 * Hi-speed access to a dictionary.
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

static void addWord(char *wd) {
	unsigned long* cur = start;
	int pos,i,numQ=0;
	char c;
	for (pos=0; (c=wd[pos]); pos++)
	{
		if (c=='q') numQ++;
		
		// End of the word.
		if (!wd[pos+1]) {
			cur[c-'a'] = cur[c-'a'] | MASK_WORD;
			if (numQ)
				cur[c-'a'] = cur[c-'a'] | MASK_Q;
			break;
		}
		else if (!(cur[c-'a'] & MASK_ADDR)) {
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
		if (wd[pos] == 'q' && wd[pos+1] == 'u')
			pos++;
	}
}

/* Actual boggle boards only have a specific number of each letter. */
//                        a b c d e f g h i j k l m n o p q r s t u v w x y z
static int maxLetCnt[] = {5,1,2,3,8,1,2,5,6,1,1,4,2,5,5,2,1,5,5,6,3,2,3,1,3,1};
static void loadAllWords(char* dict) {
	char wd[256];

	int letCnt[26];
	FILE* f = fopen(dict,"r");
	
	initNodes();
	
	int idx = 0;
	while (!feof(f) && fscanf(f,"%s",wd))
	{
		int x = wd[0] - 'a';
		if (x<0 || x>26) continue;
		
		for (int i=0; i<26;i++) letCnt[i] = 0;
		
		int hasQ = 0;
		for (int i=0; wd[i]; i++) {
			int c = wd[i] - 'a';
			if (c == 'q'-'a') hasQ=1;

			// A 'q' must be proceeded by a 'u'
			if (c == 'q'-'a' && wd[i+1] != 'u') goto eol;

			// Must be all lowercase letters
			if (c<0 || c>26) goto eol;
			
			// Check that it doesn't exceed the letter maxima
			letCnt[c]++;
			if (letCnt[c] > maxLetCnt[c]) goto eol;
			if (i>=17 || (i>=16 && !hasQ)) goto eol;
		}
		addWord(wd);
		idx++;
eol:	;
	}
	
	fclose(f);
}
