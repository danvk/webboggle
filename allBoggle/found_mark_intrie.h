#ifndef FOUND_MARK_INTRIE
#define FOUND_MARK_INTRIE

// Stores marks in the trie itself. This should be extremely fast,
// but it breaks thread-safety. Perf was only very slightly better than the
// bitvector implementation.
#define MAX_WORDS 2048
class FoundMark {
	public:
		FoundMark(Trie* t) : num_words_(0) {}

		inline void mark(Trie* t, int let) {
			t->mark(let);
			lets_[num_words_] = let;
			ts_[num_words_++] = t;
		}
		
		inline bool marked(Trie* t, int let) {
			return t->marked(let);
		}
		
		void unmark() {
			for (unsigned int i=0; i<num_words_; i++)
				ts_[i]->unmark(lets_[i]);
			num_words_ = 0;
		}
		
	private:
		unsigned int num_words_;
		unsigned int lets_[MAX_WORDS];
		Trie* ts_[MAX_WORDS];
};


#endif