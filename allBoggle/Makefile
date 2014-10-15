CC = g++
CFLAGS = -g -Wall -std=c99
#CPPFLAGS = -g -Wall -pg
CPPFLAGS = -g -Wall -O3 -march=i686
#LDFLAGS = -pg

all: test gradient trie_test partialBoggle_test webboggle

test: test.o objBoggle.o trie.o
gradient: gradient.o objBoggle.o trie.o RealBoard.o randomc/mersenne.o
trie_test: trie.o trie_test.o
partialBoggle_test: partialBoggle_test.o partialBoggle.o trie.o
webboggle: trie.o objBoggle.o

objBoggle.o: objBoggle.cpp objBoggle.h found_mark.h

clean:
	rm trie_test test partialBoggle_test gradient *.o
