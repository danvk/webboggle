CC = gcc
#CC = /usr/local/bin/gcc
#CCFLAGS = -Wall -std=c99 -O3 -fast -mcpu=7450
#CCFLAGS = -Wall -std=c99 -O3 -mdynamic-no-pic -mcpu=7450
#CCFLAGS = -Wall -std=c99 -g -pg
CFLAGS = -Wall -std=c99 -O3 -g

all: webboggle makemm allinone

webboggle: webboggle.o
makemm: makemm.o
#webboggle: webboggle.o detail.o soln.o

allinone: allinone.o detail.o soln.o
	$(CC) $(CCFLAGS) -o allinone allinone.o detail.o soln.o
	
allinone.o: allinone.c
	$(CC) $(CCFLAGS) -c allinone.c

#detail.o: detail.c allinone.h
#	$(CC) $(CCFLAGS) -c detail.c

boggle: boggle.o set.o dict.o soln.o
	$(CC) -o boggle boggle.o set.o dict.o soln.o
	
stripboggle: stripboggle.o set.o dict.o
	$(CC) -o stripboggle stripboggle.o set.o dict.o

boggle.o: boggle.c set.h
	$(CC) $(CCFLAGS) -c boggle.c
	
set.o : set.c set.h
	$(CC) $(CCFLAGS) -c set.c

dict.o : dict.c dict.h set.h
	$(CC) $(CCFLAGS) -c dict.c

#soln.o : soln.c soln.h
#	$(CC) $(CCFLAGS) -c soln.c

stripboggle.o: stripboggle.c
	$(CC) $(CCFLAGS) -c stripboggle.c
	
driver: driver.o driven.o
	$(CC) $(CCFLAGS) -o driver driver.o driven.o

driven.o: driven.c
	$(CC) $(CCFLAGS) -c driven.c
	
driver.o: driver.c boggle.h
	$(CC) $(CCFLAGS) -c driver.c

clean:
	rm *.o boggle stripboggle allinone makemm
