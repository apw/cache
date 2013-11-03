#include "../includes/in_order.h"
#include "../includes/common.h"

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

void in_order::begin_sbv(int id) {
  super::begin_sbv(id);
  vect v;
  s_[id] = v;
}

// TODO: probs don't need this
void in_order::end_sbv(int id) {
  super::end_sbv(id);
  
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

  numval nv;
  nv.bytenum = bytenum;
  nv.byteval = byteval;
  s_[id].push_back(nv);
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
  int match;
  unsigned num_matches = current_id_;
  /*
   * OPT:
   * 1. Bit-pack so things fit in cache
   * 2. call calloc once and use query/version numbers instead
   */
  int *candidates = (int *) calloc(current_id_ + 1, sizeof(int));
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
	  match = find_match_id(candidates, current_id_);
	  free(candidates);

	  int vlen = s_[match].size();
	  for(int j = 0; j < vlen; j++) {
	    if (bv[s_[match][i].bytenum] != s_[match][i].byteval) {
	      return 0;
	    }
	  }

	  return match;
	} else if (num_matches == 0) {
	  free(candidates);
	  return 0;
	}
      }

      //std::cout << "(val: " << (unsigned)b_iter->byteval << " -> id: " << b_iter->id << ")"; 
    }

    //std::cout << "]" << std::endl;
  }

  match = find_match_id(candidates, current_id_);

  free(candidates);
  return match;
}
