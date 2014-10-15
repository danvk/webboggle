#ifndef FOUND_MARK_H
#define FOUND_MARK_H

#include <vector>
using namespace std;

// Remember which words have already been found using a massive bitvector
// To speed up the clearing of this bitvector after a board has been fully
// solved, we save pointers to set bits in the bitvector. This changes perf
// from < 1,000 bds/sec to 11,000 bds/sec
#define MAX_WORDS 2048
class FoundMark {
	public:
		FoundMark(Trie* t) {
			unsigned long hi, bits;
			t->memory_span(&lo_, &hi, &bits);
			
			marks_shift_ = 0;
			while ((bits & (1<<marks_shift_)) == 0) {
				marks_shift_++;
			}
			marks_size_ = (hi-lo_)/( (1<<marks_shift_) * 8 * sizeof(unsigned long));
			marks_ = (unsigned long*)calloc(marks_size_, sizeof(long));
			num_words_ = 0;
			waterline_ = 0;
		}
   
		inline void mark(Trie* t, int let) {
			unsigned long ad = (unsigned long)&t->children[let] - lo_;
			ad >>= marks_shift_;
			marks_[ad >> 5] |= (1 << (ad&0x1F));
			words_[num_words_++] = ad>>5;
			saved_.push_back(make_pair(t, let));
		}
		
		inline bool marked(Trie* t, int let) {
			unsigned long ad = (unsigned long)&t->children[let] - lo_;
			ad >>= marks_shift_;
			return marks_[ad >> 5] & (1 << (ad&0x1F));
		}
		
		void unmark() {
			for (unsigned int i=0; i<num_words_; i++)
				marks_[words_[i]] = 0;
			num_words_ = 0;
			waterline_ = 0;
			saved_.clear();
		}
		
		/* For partial board evaluation */
		void set_waterline() { waterline_ = num_words_; }
		void unmark_waterline() {
			while (num_words_ > waterline_) {
				num_words_--;
				marks_[words_[num_words_]] = 0;
				saved_.pop_back();
			}
		}
		
		void print_words(Trie* base) {
			vector<pair<Trie*,int> >::iterator it = saved_.begin();
			for (; it != saved_.end(); ++it) {
				string wd;
				if (base->reverse_lookup(it->first, &wd)) {
					printf(" %s%c\n", wd.c_str(), 'a'+it->second);
				} else {
					printf("oops, problem\n");
				}
			}
		}
		
	private:
		unsigned int marks_shift_;
		unsigned int marks_size_;
		unsigned long lo_;
		unsigned long* marks_;
		
		unsigned int num_words_;
		unsigned int waterline_;
		unsigned int words_[MAX_WORDS];
		
		vector<pair<Trie*,int> > saved_;
};

#endif
