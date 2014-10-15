#include "soln.h"

extern int prev[4][4];

Word** sols;
int numSols;
int numAlloced;
int inited = 0;

void initSoln() {
	if (sols) {
		for (int i=0; i<numSols; i++) {
			free(sols[i]);
		}
		free(sols);
	}
	
	numAlloced = 128;
	sols = (Word**) malloc( numAlloced * sizeof(Word*) );
	numSols = 0;
	inited = 1;
}

void addSoln( char* wd, int endX, int endY ) {
	if (!inited)
		initSoln();

	int len;
	if (++numSols > numAlloced)
		sols = (Word**)realloc( sols, sizeof(Word*) * (numAlloced*=2) );
	
	sols[numSols-1] = (Word*)malloc(sizeof(Word));
	
	len = strlen(wd);
	sols[numSols-1]->wd = (char*)malloc(len*sizeof(char));
	strcpy(sols[numSols-1]->wd, wd);
	
	// Now figure out the path
	for (int n=1; n<len; n++) {
		sols[numSols-1]->path[n-1] = -1;
		for (int x=0; x<4; x++) {
			for (int y=0; y<4; y++) {
				if (prev[x][y] == n)
					sols[numSols-1]->path[n-1] = 10*y + x;
			}
		}
	}
	
	sols[numSols-1]->path[len-1] = 10*endY + endX;
	
	/*
	printf("%s\n", wd);
	for (int i=0; i<4; i++) {
		for (int j=0; j<4; j++) {
			printf(" %d", prev[i][j]);
		}
		printf("\n");
	}
	printf("%d,%d\n-------\n", endX, endY);
	*/
}

void printSolns() {
	for (int i=0; i<numSols; i++) {
		printf("%s", sols[i]->wd);
		for (int j=0; sols[i]->wd[j]; j++) {
			if (sols[i]->path[j] >= 0) 
				printf(" %02d", sols[i]->path[j]);
		}
		printf("\n");
	}
}
