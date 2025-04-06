/*
 * Print out the list of words and the paths to them as quickly
 * as possible. Uses mmap to load a specialized dictionary file.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

// mmapped dictionary
uint32_t nWds;
uint32_t* wds;
char* dict;

char bd[4][4];
int prev[4][4];

void solve(int x, int y, int n, char* wdSoFar);
void doDetailedSearch();
int strcmpPrefix(char* a, char* b);
void loadDict(char* file);
void setBd(char* lets);
int startsWord(char* key);
int isWord(char* key);

void usage_and_die(int argc, char** argv) {
	fprintf(stderr, "Usage: %s <mmapped dictionary> <catdlinemaropets>\n", argv[0]);
	exit(1);
}

int main(int argc, char** argv)
{
	if (argc != 3) usage_and_die(argc, argv);

	// need either 16 non-qs, or 17 letters with one 'qu'.
	int board_len = strlen(argv[2]);
	char* qu = strstr(argv[2], "qu");
	char* qu2 = qu ? strstr(qu + 1, "qu") : NULL;
	if (!((board_len == 16 && !qu) || (board_len == 17 && qu && !qu2))) {
		usage_and_die(argc, argv);
	}

	loadDict(argv[1]);
	setBd(argv[2]);
	doDetailedSearch();
}

void setBd(char* lets) {
	int p, i=0, j=0;
	for (p=0; lets[p]; p++) {
		bd[i][j] = lets[p];
		if (lets[p] == 'q' && lets[p+1] == 'u')
			p++;
		j++;
		if (j==4){ j=0; i++; }
	}
}

void addSoln( char* wd, int endX, int endY ) {
	int len = strlen(wd);
        for (int i = 0; i < len; i++) {
        	if (wd[i] == 'q') {
        		printf("qu");
        	} else {
        		printf("%c", wd[i]);
        	}
        }

	// Now figure out the path
	for (int n=1; n<len; n++) {
		for (int x=0; x<4; x++) {
			for (int y=0; y<4; y++) {
				if (prev[x][y] == n)
					printf(" %d%d", x, y);
			}
		}
	}

	printf(" %d%d\n", endX, endY);
}

void solve(int x, int y, int n, char* wdSoFar)
{
	int cx, cy;
	//printf("%d: %s\n", n, wdSoFar);

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

					if (startsWord(wdSoFar)) {
						if (n>=2 && isWord(wdSoFar)) {
							addSoln(wdSoFar, cx, cy);
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

void doDetailedSearch()
{
	char wd[17] = "";
	int n = 0;

	for (int i=0; i<4; i++)
		for (int j=0; j<4; j++)
			prev[i][j] = 0;

	for (int i=0; i<4; i++) {
		for (int j=0; j<4; j++) {
			wd[0] = bd[i][j];
			wd[1] = '\0';
			prev[i][j] = n;
			solve(i, j, n, wd);
			prev[i][j] = 0;
		}
	}
}

// Use mmap to load the dictionary quickly from disk
void loadDict(char* file) {
	int fd;
	struct stat fst;
	uint32_t* buf;

	fd = open(file, O_RDONLY);
	stat(file, &fst);
	buf = mmap(0, fst.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

	nWds = *buf;
	wds = buf + 1;
	dict = (char*)(buf + nWds + 1);
}

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

int isWord(char* key){
	int a, b, c, cmp;
	a = 0;
	b = nWds - 1;
	c = (b+a) / 2;

	while (b >= a) {
		cmp = strcmp(key, dict + wds[c]);
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

int startsWord(char* prefix){
	int a, b, c, cmp;
	a = 0;
	b = nWds - 1;
	c = (b+a) / 2;

	while (b >= a) {
		cmp = strcmpPrefix(prefix, dict + wds[c]);
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
