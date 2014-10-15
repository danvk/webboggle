#ifndef ALLINONE_H
#define ALLINONE_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct {
	char** dict;
	unsigned int num;
	unsigned int alloced;
} Set;

void initSet(Set*, unsigned int);
void clearSet(Set* s);
void addItem(char* key, Set* s);
void addItemNC(char* key, Set* s);
int contains(char* key, Set* s);
int containsAsPrefix(char* prefix, Set* s);

typedef Set Dict;

int isWord(char* wd);
int startsWord(char* prefix);

Dict* loadAllWords();

void freeDict(Dict* d);

int getScore();
void setBd(char* lets);

#endif
