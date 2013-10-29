#include "../includes/in_order.h"

#include <stdint.h>
#include <assert.h>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include <iostream>

#include <stdio.h>
#include <errno.h>

in_order::in_order(const char *cur_time) : rep(IN_ORDER_OUTFNAME, cur_time) {

}

in_order::~in_order() {

}

void in_order::do_add_byte(int id, unsigned bytenum, unsigned byteval) {
  if (c_.find(bytenum) == c_.end()) {
    bytepair_set bs;
    c_[bytenum] = bs;
  }
  
  bytepair bp;
  bp.id = id;
  bp.byteval = byteval;
  c_[bytenum].insert(bp);
}

int in_order::do_query(uint8_t *bv, unsigned len) {
  unsigned num_matches = current_id_ + 1;
  int *candidates = (int *) calloc(current_id_ + 1, sizeof(int));
  assert(candidates != NULL);

  cache::const_iterator c_end = c_.end();
  for(cache::const_iterator c_iter = c_.begin(); c_iter != c_end; c_iter++) {
    assert(c_iter->first < len);
    //std::cout << "bytenum: " << c_iter->first << " -> [";

    bytepair_set::const_iterator b_end = c_iter->second.end();
    for(bytepair_set::const_iterator b_iter = c_iter->second.begin(); 
	b_iter != b_end; b_iter++) {
      if (b_iter->byteval != bv[c_iter->first]) {
	if (candidates[b_iter->id] == 0) {
	  num_matches--;
	  candidates[b_iter->id] = 1;
	} else {
	  continue;
	}

	if (num_matches == 1) {
	  // TODO check the remaining vector only
	} else if (num_matches == 0) {
	  free(candidates);
	  return false;
	}
      }

      //std::cout << "(val: " << (unsigned)b_iter->byteval << " -> id: " << b_iter->id << ")"; 
    }

    //std::cout << "]" << std::endl;
  }

  int match;
  for(int i = 0; i <= current_id_; i++) {
    if (candidates[i] == 1) {
      match = i;
      break;
    }
  }
  (void)match;

  free(candidates);
  return 1;
}

