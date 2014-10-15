CC = gcc
#CC = /usr/local/bin/gcc
CFLAGS = -Wall -std=c99 -O3
#CFLAGS = -Wall -std=c99 -O3 -mdynamic-no-pic -mcpu=7450
#CCFLAGS = -Wall -std=c99 -g -pg

boggle: driver.o fastBoggle.o stdBoggle.o detail.o soln.o
	$(CC) $(CFLAGS) -o boggle $^

driver.o: driver.c

fastBoggle.o: fastBoggle.c

allinone.o: allinone.c

detail.o: detail.c

soln.o : soln.c soln.h

clean:
	rm *.o boggle
