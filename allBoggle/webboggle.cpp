#include "objBoggle.h"

int main(int argc, char** argv) {
	if (argc != 3 || strlen(argv[2]) != 16) {
		fprintf(stderr, "Usage: %s <dictionary> <catdlinemaropets>\n", argv[0]);
		exit(1);
	}
	
	if (strlen(argv[2]) != 16) {
		fprintf(stderr, "Boggle boards have sixteen letters, '%s' has %u\n",
		                 argv[2], strlen(argv[2]));
		exit(1);
	}

	Trie* t = new Trie;
	int n = t->load_file(argv[1]);
	if (n <= 0) {
		fprintf(stderr, "Couldn't load dictionary '%s'\n", argv[1]);
		exit(1);
	}
	
	Boggler* b = new Boggler(t);
	b->board_paths(argv[2]);
}
