#ifndef FOUND_MARK_HASH
#define FOUND_MARK_HASH

#include <ext/hash_set>
using namespace __gnu_cxx;

// Use a hash_set to remember which words have been found.
// This produced a Boggle algorithm that ran a little better than half as fast
// as the bitvector/saving version. (6,400 bds/sec vs. 11,000 bds/sec)
class FoundMark {
	public:
		FoundMark(Trie* t) {}

		inline void mark(Trie* t, int let) {
			unsigned long ad = (unsigned long)&t->children[let];
			marks_.insert(ad);
		}
		
		inline bool marked(Trie* t, int let) {
			unsigned long ad = (unsigned long)&t->children[let];
			return marks_.count(ad) > 0;
		}
		
		void unmark() {
			marks_.clear();
		}
		
	private:
		hash_set<unsigned long> marks_;
};


#endif