#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int* buf;
int nWds;
int* wds;
char* dict;

int main(int argc, char** argv) {
	int fd, i;
	struct stat fst;
	
	if (argc != 2){
		fprintf(stderr, "Usage: %s <memmapped file>\n", argv[0]);
		exit(1);
	}
	fd = open(argv[1], O_RDONLY);
	if (!fd) {
		fprintf(stderr, "Couldn't open file %s\n", argv[1]);
		exit(1);
	}
	stat(argv[1], &fst);

	buf = mmap(0, fst.st_size, PROT_READ, MAP_PRIVATE, fd, 0);	

	nWds = *buf;
	wds = buf + 1;
	dict = (char*)(buf + nWds + 1);

	//printf("%d, 0x%08x\n", nWds, dict);

	for (i=0; i<nWds; i++) { 
		printf("%s\n", dict+wds[i]);
	}
}
