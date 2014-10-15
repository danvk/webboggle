#ifndef SOLN_H
#define SOLN_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Data structure to hold the solutions to a boggle puzzle

typedef struct {
	char* wd;
	int path[16];
} Word;

void initSoln();
void addSoln( char* wd, int endX, int endY );
void printSolns();

#endif
