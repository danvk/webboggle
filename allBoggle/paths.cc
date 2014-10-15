#include <strings.h> // for bzero
#include <stdio.h>
#include <stdlib.h>

const int Cells = 16;
const int Size  = 4;

const bool Debug = true;

// Pack/unpack states
inline int to_idx(int prev, int x, int y) {
	if (Debug) {
		if (x < 0 || x > 3 || y < 0 || y > 3 || prev < 0 || prev > (1<<16)) {
			printf("Bad vals: %08x, %d, %d\n", prev, x, y);
		}
	}
	return (prev << 4) + (x << 2) + y;
}

inline void split_idx(int idx, int& prev, int& x, int& y) {
	prev = idx >> 4;
	x = (idx>>2) & 0x03;
	y = idx      & 0x03;
}

static inline int to_cell(int x, int y) { return (y<<2) + x; }
static inline int cell_mask(int x, int y) { return 1 << to_cell(x,y); }

// Have we calculated a particular value yet?
unsigned long calculated[(1<<Cells) * Size * Size / 32];
inline bool calced(int idx) {
	return calculated[idx >> 5] & (1<<(idx&0x1F));
}
inline void set_calced(int idx) {
	calculated[idx >> 5] |= (1<<(idx&0x1F));
}

// Cache pre-computed values
// Not everything in this table will be computed, and the simplest order is
// nontrivial, so it makes sense to use it as a cache.
unsigned long long count[65536 * 16];
unsigned long long compute_path_count(int prev, int x, int y);

inline unsigned long long path_count(int prev, int x, int y) {
	int idx = to_idx(prev, x, y);
	if (calced(idx)) return count[idx];

	unsigned long cnt = compute_path_count(prev, x, y);
	set_calced(idx);
	count[idx] = cnt;
	return cnt;
}

unsigned long long compute_path_count(int prev, int x, int y) {
	unsigned long long this_count = 0;
	int lx = x==0 ? 0 : x-1; int hx = x==3 ? 4 : x+2;
	int ly = y==0 ? 0 : y-1; int hy = y==3 ? 4 : y+2;
	
	prev |= cell_mask(x,y);
	//printf("in %04x, %d, %d\n", prev, x, y);
	
	for (int cx=lx; cx<hx; cx++) {
		for (int cy=ly; cy<hy; cy++) {
			// Make sure we haven't been here before
			if (prev & cell_mask(cx,cy)) continue;

			unsigned long long nc = path_count(prev, cx, cy);
			if (this_count + nc < this_count) {
				printf("Error, overflow w/ prev=%04X, tc=%u, nc=%u\n", prev, this_count, nc);
				exit(1);
			}
			this_count += nc;
		}
	}
	//printf("out %04x, %d, %d => %u\n", prev, x, y, this_count);
	
	prev &= ~cell_mask(x,y);
	return 1+this_count;
}

int main(int argc, char** argv) {
	// Initialize all counts to zero
	bzero(count, sizeof(count));
	bzero(calculated, sizeof(calculated));
	//for (int i=0; i<sizeof(count)/sizeof(long long); i++) {
	//	count[i] = 0;
	//}
	
	unsigned long long cnt
		= path_count(0, 0, 0) * 4
		+ path_count(0, 1, 0) * 8
		+ path_count(0, 1, 1) * 4;
	printf("Paths: %u\n", cnt);
}
