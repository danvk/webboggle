#include "stdBoggle.h"
#include "detail.h"

typedef struct {
	char** dict;
	unsigned int num;
	unsigned int alloced;
} Set;

static void initSet(Set*, unsigned int);
static void clearSet(Set* s);
static void addItem(char* key, Set* s);
static void addItemNC(char* key, Set* s);

typedef Set Dict;

static int firstLetterA[26];
static int firstLetterB[26];
static int startingPoint[26];

int findIndex(char* key);
int isWord(char* wd);
int startsWord(char* prefix);

static Dict* loadAllWords();

static void freeDict(Dict* d);

static int getScore();
static void setBd(char* lets);

static Dict* allWords;
static Dict* curDict;
static int loaded = 0;

char bd[4][4];
int prev[4][4];

static void solve(int x, int y, int n, char* wdSoFar);

static void setBd(char* lets) {
	int p, i=0, j=0;
	for (p=0; lets[p]; p++) {
		bd[i][j] = lets[p];
		if (lets[p] == 'q' && lets[p+1] == 'u')
			p++;
		j++;
		if (j==4){ j=0; i++; }
	}
}


static Set* solns; // questestaestaesta
//int main(int argc, char** argv) 
void stdBoggle(char* dict)
{
	char wd[128];
	allWords = loadAllWords(dict);
	curDict = allWords;

	//printf("'qua' => idx %d\n", findIndex("qa"));
	//printf("firstLetterA[q]=%d; firstLetterB[q]=%d\n",firstLetterA['q'-'a'],firstLetterB['q'-'a']);
	//printf("'qua' => %d\n", isWord("qa"));

	printf("Standard Boggle\n");
	fflush(stdout);

	while (scanf("%s",wd) != -1)
	{
		if (wd[0] == '>'){
			setBd(wd+1);
			doDetailedSearch();
			continue;
		}
		if (strlen(wd) < 16) break;
		setBd(wd);
		//loadDict(wd);
		printf("%d\n", getScore());
		fflush(stdout);
	}
}


int score;
int getScore() {
	solns = (Set*)malloc(sizeof(Set));
	initSet(solns, 256);
	score = 0;
	
	char wd[17] = "";
	for (int i=0; i<4; i++) {
		for (int j=0; j<4; j++) {
			wd[0] = bd[i][j];
			wd[1] = '\0';
			prev[i][j] = 1;
			solve(i, j, 1, wd);
			prev[i][j] = 0;
		}
	}
	
	clearSet(solns);
	return score;
}

int getWordScore(int n) {
	switch (n) {
		case 3: case 4: return 1;
		case 5: return 2;
		case 6: return 3;
		case 7: return 5;
		case 0: case 1: case 2: return 0;
		default: return 11;
	}
}

//awidrortsteleqush
static void addSoln(char* wd) {
	int n = solns->num;
	char** d = solns->dict;
	for (int i=0; i<n; i++) {
		if (!strcmp(d[i], wd)) {
			return;
		}
	}
	int numQ=0;
	for (int i=0; wd[i]; i++) if (wd[i]=='q') numQ++;
	//printf("%3d. %s (%d)\n", n+1, wd, getWordScore(strlen(wd)+numQ));
	addItem(wd, solns);
	//printf("adding '%s' => %d\n", wd, getWordScore(strlen(wd)+numQ));
	score += getWordScore(strlen(wd)+numQ);
	
/*	if (!strcmp(wd,"art")){
		printf("%s => ", wd);
		for (int i=0; i<solns->num; i++)
			printf("%c%s",(i ? ',' : '('), solns->dict[i]);
		printf(")\n");
	}
	*/
}

void solve(int x, int y, int n, char* wdSoFar) {
	int cx, cy;
	
	for (int dx=-1; dx<=1; dx++) {
		for (int dy=-1; dy<=1; dy++) {
			if (dx || dy) {
				cx = x + dx;
				cy = y + dy;
				
				if (cx >= 0 && cx < 4 &&
					cy >= 0 && cy < 4 &&
					!prev[cx][cy])
				{
					wdSoFar[n] = bd[cx][cy];
					wdSoFar[n+1] = '\0';
					
					//printf("%s\n", wdSoFar);
					if (startsWord(wdSoFar)) {
						//if (!strcmp(wdSoFar,"qa")) printf("found 'qa', n=%d\n",n);
						if (n>=1 && isWord(wdSoFar)) {
							//if (!strcmp(wdSoFar,"qa")) printf("found 'qa'\n");
							addSoln(wdSoFar); //addItem(wdSoFar, &soln);
						}
							
						prev[cx][cy] = 1+n;
						solve(cx, cy, n+1, wdSoFar);
						prev[cx][cy] = 0;
					}
				}
				
			}
		}
	}
}

///  set.c ////

// Initialize a set
// Allocate space for num entries
void initSet(Set* s, unsigned int alloc) {
	s->num = 0;
	s->alloced = alloc;
	s->dict = (char**) malloc( alloc * sizeof(char*) );
}

void clearSet(Set* s) {
	for (int i=0; i<s->num; i++)
		free(s->dict[i]);
	free(s->dict);
	s->alloced = 0;
	s->num = 0;
}

// Add an item to the end of the set
// Copy the string passed as a param
void addItem(char* key, Set* s) {
	char* tmp = strdup(key);
	addItemNC( tmp, s );
}

// Add an item, w/o copying
void addItemNC(char* key, Set* s) {
	if (++s->num > s->alloced) {
		s->alloced <<= 1;
		s->dict = (char**) realloc( s->dict, s->alloced * sizeof(char*) );
	}
	s->dict[s->num-1] = key;
}

// Do a binary search for an item in the set
// assume that the set is in alphabetical order
// could rewrite to use the standard C function

// Modified strcmp to only compare up through
// the end of a. This is not symmetric!
int strcmpPrefix(char* a, char* b) {
	int i; char d;
	for (i=0; a[i]; i++) {
		d = a[i] - b[i];
		if (d < 0) return -1;
		if (d > 0) return  1;
	}
	return 0;
}

void gnomesort(int n, char** ar) {
	int i = 0;
	
	while (i < n) {
		if (i == 0 || strcmp(ar[i-1], ar[i]) <= 0) i++;
		else {char* tmp = ar[i]; ar[i] = ar[i-1]; ar[--i] = tmp;}
	}
}

void sortSet(Set* s) {
	//mergesort( s->dict, s->num, sizeof(char*), (int (*)(void *, void *))strcmp );
	gnomesort(s->num, s->dict);
}

///  dict.c  ///

// For debugging
int findIndex(char* key) {
	int i;
	for (i=0; i<curDict->num; i++) {
		if (!strcmp(curDict->dict[i],key))
			return i;
	}
	return -1;
}

int isWord(char* key){
	int a, b, c, cmp;
	a = firstLetterA[key[0]-'a']; //0;
	b = firstLetterB[key[0]-'a']; //s->num-1;
	//a = 0;
	//b = curDict->num-1;
	
	//c = startingPoint[key[0]-'a'];
	//if (c==-1)
		c = (b+a) / 2;
	//c = firstLetterOffsets[key[0] - 'a'];
	while (b >= a) {
		cmp = strcmp(key, curDict->dict[c]);
		//printf("%s cmp %s = %u\n", key, s->dict[c], cmp);
		if (cmp < 0) {
			b = c-1;
		} else if (cmp > 0) {
			a = c+1;
		} else {
			//startingPoint[key[0]-'a'] = c;
			return 1;
		}
		c = (b+a) / 2;
	}
	
	return 0;
}

int startsWord(char* prefix){
	int a, b, c, cmp;
	int i; char d; char *bc;
	a = firstLetterA[prefix[0]-'a'];
	b = firstLetterB[prefix[0]-'a'];
	//a=0;
	//b=curDict->num-1;
	
	c = (a+b)>>1;
	
	while (b >= a) {
		cmp = 0;
		bc = curDict->dict[c];
		for (i=0; prefix[i]; i++) {
			d = prefix[i] - bc[i];
			if (d < 0){ cmp = -1; break; }
			if (d > 0){ cmp = +1; break; }
		}
			
		if (cmp < 0) {
			b = c-1;
		} else if (cmp > 0) {
			a = c+1;
		} else {
			return 1;
		}
		c = (b+a) / 2;
	}
	
	return 0;
}

//                 a b c d e f g h i j k l m n o p q r s t u v w x y z
int maxLetCnt[] = {5,1,2,3,8,1,2,5,6,1,1,4,2,5,5,2,1,5,5,6,3,2,3,1,3,1};
Dict* loadAllWords(char* dict) {
	char wd[256];
	char newWd[20];
	//int firstInLet[26], lastInLet[26];
	int letCnt[26];
	FILE* f = fopen(dict,"r");
	Dict* s = (Dict*)malloc(sizeof(Dict));
	
	for (int i=0; i<26; i++){ firstLetterA[i] = firstLetterB[i] = startingPoint[i] = -1; }
	
	initSet(s, 4096);
	
	int idx = 0;
	while (!feof(f) && fscanf(f,"%s",wd))
	{
		int x = wd[0] - 'a';
		int d = 0;
		if (x<0 || x>26) continue;
		
		for (int i=0; i<26;i++) letCnt[i] = 0;
		
		int hasQ = 0,i;
		for (i=0; wd[i]; i++) {
			int c = wd[i] - 'a';
			if (c == 'q'-'a'){
				hasQ=1;
				if (wd[i+1] == 'u')
					i++;
				else
					goto eol;
			}
					
			if (c<0 || c>26) goto eol;
			letCnt[c]++;
			if (letCnt[c] > maxLetCnt[c]) goto eol;
			if (i>=17 || (i>=16 && !hasQ)) goto eol;
			newWd[d++] = c+'a';
		}
		newWd[d] = '\0';
		if (d+hasQ >= 3) {
			//if (x=='q'-'a') printf("'%s' => '%s'\n", wd, newWd);
			//if (!strcmp(wd,"qua"))
			//	printf("adding 'qua'=>'%s', firstLetterA[q]=%d, idx=%d\n", newWd, firstLetterA[x],idx);
			addItem(newWd, s);
			if (firstLetterA[x] == -1) firstLetterA[x] = idx;
			if (firstLetterB[x] < idx) firstLetterB[x] = idx;
			idx++;
		}
eol:	;
	}
	
	fclose(f);

	loaded = 1;	
	return s;
}
