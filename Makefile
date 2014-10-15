CC = gcc
CFLAGS = -Wall -std=c99 -O3 -g

all: webboggle makemm scrabble-words.mmapped

webboggle: webboggle.o
makemm: makemm.o

scrabble-words.mmapped: makemm scrabble-words.txt
	./makemm scrabble-words.txt scrabble-words.mmapped

clean:
	rm *.o webboggle makemm scrabble-words.mmapped
