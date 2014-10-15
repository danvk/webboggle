/*
 * Load the legal Boggle words from a dictionary file into memory, and
 * dump out to a file suitable for mmapping from disk.
 * Format of the dump:
 *   <length of word list, 4 bytes>
 *   <positions of words, 4 bytes each>
 *   <words, null-terminated, unaligned>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int pos;
char* buf;
int bufSize;

int* wds;
int nWds, nWdB;

void loadAllWords(char* dict);

int main(int argc, char** argv) {
	FILE* f;

	if (argc != 3) {
		fprintf(stderr, "Usage: %s <dictionary> <dump file>\n", argv[0]);
		exit(1);
	}

	nWdB = 1<<14;
	nWds = 0;
	wds = (int*)malloc(nWdB * sizeof(int));

	pos = 0;
	bufSize = 1<<20;
	buf = (char*)malloc(bufSize * sizeof(char));

	loadAllWords(argv[1]);

	printf("pos/bufSize: %d/%d\n", pos, bufSize);
	printf("nWds: %d\n", nWds);

	// Write out the dump
	f = fopen(argv[2], "w");
	fwrite(&nWds, sizeof(int), 1, f);
	fwrite(wds, sizeof(int), nWds, f);
	fwrite(buf, sizeof(char), pos, f);
	fclose(f);
}

// Copy the string passed as a param
void addItem(char* key) {
	// Resize buffer as necessary
	if (pos + strlen(key) >= bufSize) {
		bufSize *= 2;
		buf = (char*) realloc(buf, bufSize * sizeof(char));
	}

	// Make a note of where this word starts
	if (nWdB < nWds + 1) {
		nWdB *= 2;
		wds = (int*) realloc(wds, nWdB*sizeof(int));
	}
	wds[nWds++] = pos;
	
	for (; *key; key++){
		buf[pos++] = *key;
		if (*key == 'q' && key[1] ==  'u')
			key++;
	}
	buf[pos++] = '\0';
}

//                 a b c d e f g h i j k l m n o p q r s t u v w x y z
int maxLetCnt[] = {5,1,2,3,8,1,2,5,6,1,1,4,2,5,5,2,1,5,5,6,3,2,3,1,3,1};
void loadAllWords(char* dict) {
	char wd[256];
	int letCnt[26];
	FILE* f = fopen(dict,"r");
	
	int idx = 0;
	while (!feof(f) && fscanf(f,"%s",wd))
	{
		int i, x = wd[0] - 'a';
		if (x<0 || x>26) continue;
		
		for (i=0; i<26;i++) letCnt[i] = 0;
		
		int hasQ = 0;
		for (i=0; wd[i]; i++) {
			int c = wd[i] - 'a';
			if (c == 'q'-'a') hasQ=1;
			if (c<0 || c>26) goto eol;
			letCnt[c]++;
			if (letCnt[c] > maxLetCnt[c]) goto eol;
			if (i>=17 || (i>=16 && !hasQ)) goto eol;
		}
		if (i < 3) continue;
		addItem(wd);
		idx++;
eol:	;
	}
	
	//printf("Loaded %d words\n", s->num);
	
	fclose(f);
}

