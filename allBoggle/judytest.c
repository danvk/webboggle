#include <Judy.h>
#include <stdio.h>

#define BAD_IDX 300000

void test(int N) {
	Pvoid_t jar = (Pvoid_t) NULL;
	int ret;
	int i;
	J1S(ret, jar, BAD_IDX);
	
	for (i=0; i< N; i++) { // 141 doesn't work
		J1S(ret, jar, i);
	}
	
	J1T(ret, jar, BAD_IDX);
	if (ret != 1) {
		printf("bug! bug!, returned %d but expected 1 with %d\n", ret, N);
	} else {
		printf("OK with %d\n", N);
	}
	J1FA(ret, jar);
}

int main(int argc, char** argv) {
	test(41);
	test(42);
	test(31);
	test(100);
	test(2000);
}
