#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <stdio.h>
#include <stdlib.h>

void loadAllWords(char* dict);
int isWord(char* wd, unsigned long* cur);
int startsWord(char* wd, unsigned long* cur);

#define MASK_ADDR 0xFFFFFFF0
#define MASK_WORD 0x00000001
#define MASK_USED 0x00000002
#define MASK_Q	  0x00000004

#endif
