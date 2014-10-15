CC = gcc
CFLAGS = -Wall -std=c99 -O3 -g

all: webboggle makemm

webboggle: webboggle.o
makemm: makemm.o

clean:
	rm *.o webboggle makemm
