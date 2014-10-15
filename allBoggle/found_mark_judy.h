#ifndef FOUND_MARK_JUDY
#define FOUND_MARK_JUDY

#include <Judy.h>

// Use a hash_set to remember which words have been found.
// This produced a Boggle algorithm that ran a little better than half as fast
// as the bitvector/saving version. (6,400 bds/sec vs. 11,000 bds/sec)
class FoundMark {
	public:
		FoundMark(Trie* t) : marks_((Pvoid_t)NULL) {}

		inline void mark(Trie* t, int let) {
			unsigned long ad = (unsigned long)&t->children[let];
			int ret;
			printf("setting 0x%08x\n", ad);
			J1S(ret, marks_, ad);
		}
		
		inline bool marked(Trie* t, int let) {
			unsigned long ad = (unsigned long)&t->children[let];
			int ret;
			printf("testing 0x%08x\n", ad);
			J1T(ret, marks_, ad);
			return ret;
		}
		
		void unmark() {
			int ret;
			J1FA(ret, marks_);
		}
		
	private:
		Pvoid_t marks_;
};


#endif