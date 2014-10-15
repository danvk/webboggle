#ifndef TRIE_H
#define TRIE_H

#include <string>
using namespace std;

class Trie {
	public:
		static const unsigned int MASK_WORD = 0x01;
		static const unsigned int MASK_USED = 0x02;
		static const unsigned int MASK_ADDR = ~(MASK_WORD | MASK_USED);
		unsigned long children[26];
		//Trie* children[26];
		//bool isword[26];
		
		// Direct access: FAST
		//  0 <= let < 26
		inline Trie* descend(int let) {
			return (Trie*)(children[let] & MASK_ADDR);
			//return children[let];
		}
		
		inline bool is_word(int let) {
			return children[let] & MASK_WORD;
			//return isword[let];
		}
		
		inline bool starts_word(int let) {
			return children[let] & MASK_ADDR;
			//return children[let] != NULL;
		}
		
		/*
		// Note: these functions modify the trie! not threadsafe!!
		inline void   mark(int let) { children[let] |=  MASK_USED; }
		inline void unmark(int let) { children[let] &= ~MASK_USED; }
		
		inline bool marked(int let) {
			return children[let] & MASK_USED;
		}
		*/
		
		// Analysis
		// Returns the node containing the last letter of wd as a child
		Trie* descend_to(char* wd);
		bool is_word(const char* wd);
		
		// These functions all require a full Trie reversal and can be very slow
		void print();
		void memory_span(unsigned long* lo, unsigned long* hi, unsigned long* bits);
		int mem_usage();
		bool reverse_lookup(Trie* t, string* out);
		
		// Creation: speed is unimportant
		Trie();
		void add_word(char* wd);
		int load_file(char* dict);
		
	private:
		void print_help(char* buf, int len);
};

#endif