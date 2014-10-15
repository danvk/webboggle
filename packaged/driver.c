#include <unistd.h>
#include <string.h>
#include <stdio.h>

/*
There's a discrepancy:
 run it on clmdmaropetsline
*/

#include "fastBoggle.h"
#include "stdBoggle.h"

void usage(char*);

int main(int argc, char** argv) {
	int ch;
	int useFast = 0;
	char* dict = "/usr/share/dict/words";
	
	while ((ch = getopt(argc, argv, "sfd:?")) != -1)
	{
		switch (ch) {
			case 'f':
				useFast = 1;
				break;
			
			case 's':
				useFast = 0;
				break;
				
			case 'd':
				dict = strdup(optarg);
				break;
				
			case '?':
				usage(argv[0]);
				exit(0);
		}
	}
	
	if (useFast) {
		fastBoggle(dict);
	} else {
		stdBoggle(dict);
	}
}

void usage(char* prog) {
	printf("Usage: %s [-fs] [-d dictionary]\n", prog);
	printf("\t-f\tUses an alternative, faster algorithm that consumes more memory.\n");
	printf("\t-s\tUses the standard slower, less memory-hungry algorithm.\n");
	printf("\t-d\tSpecify an alternative dictionary.\n\n");
}
