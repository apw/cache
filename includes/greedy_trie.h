#ifndef _GREEDY_TRIE_H
#define _GREEDY_TRIE_H

#include "rep.h"
#include "in_order.h"
#include "uset_uint.h"
#include "d_trie.h"
#include <stdint.h>
#include <iostream>
#include <tr1/unordered_map>
#include <tr1/unordered_set>
#include <vector>

using namespace std;

class greedy_trie : public in_order {
 public:
  greedy_trie(const char *cur_time);
  ~greedy_trie(void);
  
  void prepare_to_query(void);
  unsigned do_query(uint8_t *bv, unsigned len);

  void viz(void);

 protected:
  int get_max_bytenum(uset_uint *done, uset_uint *u, uset_uint *bytenums_left);
  void populate_subtrie(d_trie *d, uset_uint *done,
			uset_uint *u, uset_uint *bytenums_left);
  
  unsigned do_query_helper(d_trie *current, uint8_t *bv, unsigned len);

  d_trie *d_;
  
  unsigned max_relevant_bytenum_;

 private:
  typedef in_order super;
};

#endif
