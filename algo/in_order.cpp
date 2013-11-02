#include "../includes/in_order.h"

#include <stdint.h>
#include <assert.h>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include <iostream>

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

in_order::in_order(const char *cur_time) : rep(IN_ORDER_OUTFNAME, cur_time) {
  relevant_ = NULL;
  num_relevant_ = 0;
}

in_order::~in_order() {
  free(relevant_);
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

void in_order::prepare_to_query() {
  num_relevant_ = c_.size();
  relevant_ = (unsigned *) malloc(sizeof(unsigned) * num_relevant_);
  assert(relevant_ != NULL);

  unsigned i = 0;
  cache::const_iterator c_end = c_.end();
  for(cache::const_iterator c_iter = c_.begin(); c_iter != c_end; c_iter++) {
    relevant_[i] = c_iter->first;
    i++;
  }
}

int in_order::do_query(uint8_t *bv, unsigned len) {
  unsigned num_matches = current_id_;
  /*
   * OPT:
   * 1. Bit-pack so things fit in cache
   * 2. call calloc once and use query/version numbers instead
   */
  int *candidates = (int *) calloc(current_id_, sizeof(int));
  assert(candidates != NULL);

  for(unsigned i = 0; i < num_relevant_; i++) {
    assert(relevant_[i] < len);

    bytepair_set::const_iterator b_end = c_[relevant_[i]].end();
    for(bytepair_set::const_iterator b_iter = c_[relevant_[i]].begin(); 
	b_iter != b_end; b_iter++) {
      if (b_iter->byteval != bv[relevant_[i]]) {
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
	  return 0;
	}
      }

      //std::cout << "(val: " << (unsigned)b_iter->byteval << " -> id: " << b_iter->id << ")"; 
    }

    //std::cout << "]" << std::endl;
  }

  int match = 0;
  for(int i = 0; i < current_id_; i++) {
    if (candidates[i] == 1) {
      match = i;
      break;
    }
  }

  free(candidates);
  return match;
}

