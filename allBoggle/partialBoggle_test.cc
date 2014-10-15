#include "partialBoggle.h"

double secs() {
  struct timeval t;
  gettimeofday(&t, NULL);
  return t.tv_sec + t.tv_usec / 1000000.0;
}

int main(int argc, char** argv) {
	Trie* t = new Trie;
	int words = t->load_file("words");
	printf("Loaded %d words\n", words);
	
	PartialBoard pbd;
	PartialBoggler pbog(t);
	double realstart = secs();
	//char buf[25] = "catdlinemaropets";
	if (pbog.partial_solve("catdlinemaropets", &pbd)) {
		//pbd.print_debug(t, 5);
		//printf("changing position 5 to 'c' to introduce 'ace'\n");
		
		int tally = 0;
		double start = secs();
		for (int pos=0; pos<16; pos++) {
			//char p = buf[pos];
			for (int let=0; let<26; let++) {
				int score = pbog.solve(pbd, pos, 'a'+let);
				tally += score;
				//buf[pos] = 'a' + let;
				//printf("%s\n", buf);
				//buf[pos] = p;
				//printf("'abcde%cghijklmnop' scored %d\n", 'a'+i, score);
			}
		}
		double end = secs();
		printf("Time elapsed: %f millis\n", (end-start)*1000.0);
		printf("Speed: %f bds/sec\n", (26.0*16.0)/(end-start));
		
		printf("Full time elapsed: %f millis\n", (end-realstart)*1000.0);
		printf("Speed: %f bds/sec\n", (26.0*16.0)/(end-realstart));
		
		printf("Average score: %.2f\n", tally / (26.0*16.0));
	} else {
		printf("failed\n");
	}
}