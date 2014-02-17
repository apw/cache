#include "../includes/rep.h"
#include "../includes/in_order.h"
#include "../includes/simple_cb.h"
#include "../includes/common.h"

#include <iostream>
#include <vector>
#include <algorithm>

#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

simple_cb::simple_cb(const char *cur_time) 
  : rep(cur_time, "simple_cb"), in_order(cur_time) {

}

void simple_cb::order_bytenums() {
  vector<bytenum_prio> bp_arr;
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

    bytenum_prio bp;
    bp.bytenum = c_iter->first;
    bp.prio = val_set.size();
    bp_arr.push_back(bp);
  }

  std::sort(bp_arr.begin(), bp_arr.end(), prio_compare);

  num_relevant_ = c_.size();
  relevant_ = (unsigned *) malloc(sizeof(unsigned) * num_relevant_);
  assert(relevant_ != NULL);

  unsigned i = 0;
  vector<bytenum_prio>::iterator bp_end = bp_arr.end();
  for (vector<bytenum_prio>::iterator bp_iter = bp_arr.begin();
       bp_iter != bp_end; bp_iter++) {
    //std::cout << bp_iter->prio << " " << bp_iter->bytenum << std::endl; // !!!
    relevant_[i] = bp_iter->bytenum;
    i++;
  }
}

void simple_cb::prepare_to_query() {
  candidates_ = new rset_uint(current_id_ + 1);
  
  order_bytenums();
}
