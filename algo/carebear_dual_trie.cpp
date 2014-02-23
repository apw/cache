#include "../includes/trie_cb.h"
#include "../includes/carebear_dual_trie.h"
#include "../includes/uset_uint.h"
#include "../includes/common.h"

#include <assert.h>
#include <algorithm>

carebear_dual_trie::carebear_dual_trie(const char *cur_time)
  : rep(cur_time, "carebear_dual_trie"), trie_cb(cur_time) {
  
}

void carebear_dual_trie::order_bytenums() {
  vector<carebear_prio> bp_arr;
  cache::const_iterator c_end = c_.end();
  for(cache::const_iterator c_iter = c_.begin(); c_iter != c_end; c_iter++) {
    tr1::unordered_set<uint8_t> val_set;
    bytenum_set::const_iterator b_end = c_[c_iter->first].end();
    for(bytenum_set::const_iterator b_iter = c_[c_iter->first].begin(); 
	b_iter != b_end; b_iter++) {
      if (val_set.count(b_iter->second) == 0) {
	val_set.insert(b_iter->second);
      }
    }

    carebear_prio bp;
    bp.bytenum = c_iter->first;
    bp.variability = val_set.size();
    bp.num_care = c_[c_iter->first].size();
    bp_arr.push_back(bp);
  }
  
  std::sort(bp_arr.begin(), bp_arr.end(), carebear_compare);

  num_relevant_ = c_.size();
  relevant_ = (unsigned *) malloc(sizeof(unsigned) * num_relevant_);
  assert(relevant_ != NULL);

  unsigned i = 0;
  vector<carebear_prio>::iterator bp_end = bp_arr.end();
  for (vector<carebear_prio>::iterator bp_iter = bp_arr.begin();
       bp_iter != bp_end; bp_iter++) {    
    //    std::cout << "num_care: " << bp_iter->num_care << " variability: " << bp_iter->variability << " bytenum: " << bp_iter->bytenum << std::endl; // !!!
    
    relevant_[i] = bp_iter->bytenum;
    i++;
  }
}
