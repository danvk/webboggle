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

#include "dictionary.h"

unsigned long start[26];

// ^- Not used yet, will keep track of whether a word has been found.

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
		
		//printf("%lu\n", cur[c-'a']);
		if (!wd[pos+1]) {
			cur[c-'a'] = cur[c-'a'] | MASK_WORD;
			if (numQ)
				cur[c-'a'] = cur[c-'a'] | MASK_Q;
			break;
		}
		else if (!(cur[c-'a'] & MASK_ADDR)) {
			//printf("Creating %s (0:%d)\n", wd, pos);
			unsigned long save = cur[c-'a'] & MASK_WORD;
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

int isWord(char* wd, unsigned long* cur) {
	int i;
	for (i=0; wd[i+1]; i++) {
		cur = (unsigned long*)cur[wd[i]-'a'];
		if (!cur) return 0;
		cur = (unsigned long*)( (unsigned long)cur & MASK_ADDR );
	}
	return (cur[wd[i]-'a'] & MASK_WORD);
}

int startsWord(char* wd, unsigned long* cur) {
	int i;
	for (i=0; wd[i]; i++) {
		cur = (unsigned long*)(cur[wd[i]-'a'] & MASK_ADDR);
		if (!cur) return 0;
	}
	return 1;
}

//                 a b c d e f g h i j k l m n o p q r s t u v w x y z
//static char dict[] = "/usr/share/dict/words";
int maxLetCnt[] = {5,1,2,3,8,1,2,5,6,1,1,4,2,5,5,2,1,5,5,6,3,2,3,1,3,1};
void loadAllWords(char* dict) {
	char wd[256];
	//int firstInLet[26], lastInLet[26];
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
			if (c<0 || c>26) goto eol;
			letCnt[c]++;
			if (letCnt[c] > maxLetCnt[c]) goto eol;
			if (i>=17 || (i>=16 && !hasQ)) goto eol;
		}
		addWord(wd);
		idx++;
eol:	;
	}
	
	//printf("Dictionary structure: %u bytes\n", amountAlloced);
	
	fclose(f);
}

/*
int main(int argc, char** argv) {
	char x[256];
	loadAllWords();
	printf("Allocated: %d bytes\n", amountAlloced);
	while (scanf("%s",x) != -1) {
		printf("isWord('%s') = %d; startsWord: %d\n",
				x, isWord(x,start), startsWord(x,start));
	}
}
*/