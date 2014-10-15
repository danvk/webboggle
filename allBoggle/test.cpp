#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include <string>       // for strings
#include <iostream>     // for I/O
#include <fstream>      // for file I/O
using namespace std;

#include "objBoggle.h"
#include "trie.h"

double secs();
void test(Boggler* b, const char* bd, int expect);
void test_wordcount(Boggler* b, const char* bd, unsigned expect);
void run_test_file(Boggler* b, char* filename);
void run_perf_test(Boggler* b, char* filename);

int main(int argc, char** argv) {
	Trie* t = new Trie; // REALLY important that this be on the heap
	
	int n = t->load_file("words");
	printf("loaded %d words\n", n);

	Boggler* b = new Boggler(t);

	printf("Testing correctness...\n");
	/*
	test_wordcount(b, "abcdefghijklmnop", 16);
	test_wordcount(b, "catdlinemaropets", 773);
	test_wordcount(b, "catdlinemaropets", 773);
	test_wordcount(b, "abcdefghijklmnop", 16);
	*/
	run_test_file(b, "tests");
	run_test_file(b, "bigtests");
	run_test_file(b, "tests");

	printf("Testing performance...\n");
	run_perf_test(b, "50kBoards.txt");
	//run_perf_test(b, "catdperms");
}

// Test a score against expectations
void test(Boggler* b, const char* bd, int expect) {
	int sc = b->board_score(bd);
	//int sc = b->solve_with_dfs(bd);
	if (sc != expect) {
		printf("%s => %d (expect %d) FAIL\n", bd, sc, expect);
		b->board_trace(bd);
		vector<string> words;
		b->board_words(bd, &words);
		sort(words.begin(), words.end());
		for (unsigned i=0; i<words.size(); i++)
			printf("%s\n", words[i].c_str());
		exit(1);
	}
}

// Test wordcount against expectations
void test_wordcount(Boggler* b, const char* bd, unsigned expect) {
	vector<string> words;
	b->board_words(bd, &words);
	if (words.size() != expect) {
		for (unsigned i=0; i<words.size(); i++)
			printf("%s\n", words[i].c_str());
	}
	if (words.size() != expect) {
		printf("%s => %d (expect %d) FAIL\n", bd, (int)words.size(), expect);
		exit(1);
	}

}

void run_test_file(Boggler* b, char* filename) {
    ifstream file(filename);
	if (!file) {
		fprintf(stderr, "couldn't open %s\n", filename);
		exit(1);
	}
	string line;
	string score;
	while (file >> line && file >> score) {
		int sc = atoi(score.c_str());
		test(b, line.c_str(), sc);
	}
}

void run_perf_test(Boggler* b, char* filename) {
	ifstream file(filename);
	if (!file) {
		fprintf(stderr, "couldn't open %s\n", filename);
		exit(1);
	}
	string line;
	vector<string> bds;
	while (file >> line) {
		bds.push_back(line);
	}
	
	printf("Testing perf on %d boards...\n", (int)bds.size());
	vector<string>::iterator it = bds.begin();
	vector<string>::iterator e = bds.end();
	double start = secs();
	for (; it != e; ++it) {
		b->board_score(it->c_str());
	}
	double end = secs();
	
	printf("%d boards in %.5f secs => %.2f bds/sec\n",
			(int)bds.size(), end-start, 1.0*bds.size()/(end-start));

	printf("Average words/board: %.4f wds/bd\n", 1.0*b->num_words()/b->num_boards());
	printf("Average board score: %.4f pts/bd\n", 1.0*b->num_points()/b->num_boards());
}

double secs() {
  struct timeval t;
  gettimeofday(&t, NULL);
  return t.tv_sec + t.tv_usec / 1000000.0;
}
