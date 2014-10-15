#include "trie.h"

int main(int argc, char** argv) {
	Trie* t = new Trie;
	int words = t->load_file("words");
	
	printf("Loaded %d words\n", words);
	printf("Using %d bytes of memory\n", t->mem_usage());
	
	Trie* node = t->descend_to("unsigned");
	if (!node) {
		printf("error, couldn't find 'unsigned' in Trie\n");
	} else {
		string out;
		if (t->reverse_lookup(node, &out)) {
			printf("Reverse lookup from 'unsigned' => '%s'\n", out.c_str());
		} else {
			printf("Reverse lookup from 'unsigned' failed\n");
		}
	}
}
