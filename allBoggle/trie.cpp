#include "trie.h"
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <stack>

Trie::Trie() {
	bzero(children, 26 * sizeof(unsigned long));
}

void Trie::add_word(char* wd) {
	if (wd[0] == '\0') {
		fprintf(stderr, "shouldn't happen...\n");
		return;
	}
	
	int let = wd[0] - 'a';
	if (wd[1] == '\0') {
		children[let] |= MASK_WORD;
		//isword[let] = true;
	} else {
		if (!starts_word(let)) {
			unsigned long info = children[let];
			children[let] = (unsigned long)(new Trie);
			children[let] |= info;
			//children[let] = new Trie;
		}
		if (wd[0] == 'q' && wd[1] == 'u') {
			descend(let)->add_word(wd+2);
		} else {
			descend(let)->add_word(wd+1);
		}
	}
}

int Trie::load_file(char* dict) {
	  FILE* f = fopen(dict,"r");
	  if (!f) {
		fprintf(stderr, "Couldn't open dictionary file '%s'\n", dict);
		return -1;
	  }
	
	  char wd[256];
	  int numWords = 0;
	  while (!feof(f) && fscanf(f,"%s",wd))
	  {
		// Screen for valid Boggle words.
		// These consist only of lowercase letters.
		// All q's must be immediately followed by u's
		int bad = 0;
		for (int i=0; wd[i]; i++) {
		  if (wd[i]<'a' || wd[i]>'z') bad=1;
		  if (wd[i]=='q' && wd[i+1]!='u') bad=1;
		}
		if (bad) continue;
		
		add_word(wd);
		numWords++;
	  }
	  fclose(f);
	  return numWords;
}

void Trie::print_help(char* buf, int len) {
	for (int i=0; i<26; i++) {
		if (is_word(i)) {
			printf("%s%c\n", buf, 'a'+i);
		}
		if (starts_word(i)) {
			buf[len] = 'a' + i;
			buf[len+1] = '\0';
			descend(i)->print_help(buf, len+1);
			buf[len] = '\0';
		}
	}
}
void Trie::print() {
	int len = 0;
	char buf[256] = "";
	print_help(buf, len);
}

Trie* Trie::descend_to(char* wd) {
	Trie* ret = this;
	while (wd[1] != '\0') {
		if (ret->starts_word(wd[0] - 'a')) {
			ret = ret->descend(wd[0] - 'a');
			wd++;
		} else {
			return NULL;
		}
	}
	return ret;
}

bool Trie::is_word(const char* wd) {
	if (!wd || wd[0] == '\0' || wd[0] < 'a' || wd[0] > 'z') {
		return false;
	}
	
	if (wd[1] == '\0')
		return is_word(wd[0] - 'a');
	else
		return starts_word(wd[0]-'a') && descend(wd[0]-'a')->is_word(wd+1);
}

void Trie::memory_span(unsigned long* lo, unsigned long* hi, unsigned long* bits) {
	*hi = 0;
	*lo = (unsigned long)-1L;
	*bits = 0;
	stack<Trie*> st;
	
	st.push(this);
	while (!st.empty()) {
		Trie* t = st.top();
		st.pop();
		
		unsigned long lo_ad = (unsigned long)t;
		unsigned long hi_ad = (unsigned long)&t->children[25];
		if (*lo > lo_ad) *lo = lo_ad;
		if (*hi < hi_ad) *hi = hi_ad;
		*bits |= lo_ad;
		
		for (int i=0; i<26; i++) {
			unsigned long ad = (unsigned long)&t->children[i];
			*bits |= ad;
			if (t->starts_word(i))
				st.push(t->descend(i));
		}
	}
}

int Trie::mem_usage() {
	int usage = sizeof(Trie);
	for (int i=0; i<26; i++) {
		if (starts_word(i))
			usage += descend(i)->mem_usage();
	}
	return usage;
}

bool Trie::reverse_lookup(Trie* target, string* out) {
	// Agh functors are AWKWARD!!!
	struct {
		bool operator()(Trie* t, Trie* target) {
			if (t == target)
				return true;
			for (int i=0; i<26; i++) {
				if (t->starts_word(i)) {
					if (operator()(t->descend(i), target)) {
						answer = string(1,'a'+i) + answer;
						return true;
					}
				}
			}
			return false;
		}
		string answer;
	} helper;
	
	if (helper(this, target)) {
		*out = helper.answer;
		return true;
	}
	return false;
}
