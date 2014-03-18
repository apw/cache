#include "../includes/rep.h"
#include "../includes/in_order.h"
#include "../includes/greedy_forest.h"
#include "../includes/common.h"

#include <iostream>
#include <vector>
#include <algorithm>

#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <pthread.h>

greedy_forest::greedy_forest(const char *cur_time) 
  : rep(cur_time, "greedy_forest"), carebear_forest(cur_time) {
  
}

int greedy_forest::get_max_bytenum(uset_uint *done, uset_uint *u, uset_uint *bytenums_left) {
  unsigned max_variability = 0;
  unsigned max_num_care = 0;
  int max_bytenum = -1;
  cache::const_iterator c_end = c_.end();
  for(cache::const_iterator c_iter = c_.begin(); c_iter != c_end; c_iter++) {
    if (bytenums_left->lookup(c_iter->first)) {
      unsigned cur_num_care = 0;
      tr1::unordered_set<uint8_t> val_set;
      bytenum_set::const_iterator b_end = c_[c_iter->first].end();
      for(bytenum_set::const_iterator b_iter = c_[c_iter->first].begin(); 
	  b_iter != b_end; b_iter++) {

	if (done->lookup(b_iter->first) && u->lookup(b_iter->first)) {
	  cur_num_care++;
	  
	  if (val_set.count(b_iter->second) == 0) {
	    val_set.insert(b_iter->second);
	  }
	}
      }

      unsigned cur_variability = val_set.size();
      if (cur_variability > max_variability 
	  || (cur_variability == max_variability && cur_num_care > max_num_care)) {
	max_variability = cur_variability;
	max_bytenum = c_iter->first;
	max_num_care = cur_num_care;
      }
    }
  }
  
  if (max_bytenum != -1) {
    bytenums_left->begin_trans();
    bytenums_left->remove(max_bytenum);
    bytenums_left->end_trans();
  }
  
  return max_bytenum;
}
