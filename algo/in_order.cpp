#include "../includes/in_order.h"
#include "../includes/common.h"
#include "../includes/rset_uint.h"

#include <stdint.h>
#include <assert.h>
#include <iostream>
#include <vector>

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

in_order::in_order(const char *cur_time) : rep(cur_time, "in_order") {
  relevant_ = NULL;
  num_relevant_ = 0;
  candidates_ = NULL;
}

in_order::~in_order() {
  free(relevant_);
  delete candidates_;
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
    bytenum_set bs;
    c_[bytenum] = bs;
  }
  
  c_[bytenum][id] = byteval;

  numval nv;
  nv.bytenum = bytenum;
  nv.byteval = byteval;
  s_.resize(id+1);
  s_[id].push_back(nv);
}

void in_order::prepare_to_query() {
  candidates_ = new rset_uint(current_id_ + 1);

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

unsigned in_order::do_query(uint8_t *bv, unsigned len) {
  candidates_->restore();

  unsigned cur_id;
  for(unsigned i = 0; i < num_relevant_; i++) {
    assert(relevant_[i] < len);

    for(rset_uint::iterator bs_iter = candidates_->get_iterator();
	bs_iter.is_cur_valid();) {

      num_steps_++;

      cur_id = bs_iter.get_cur();

      if (c_[relevant_[i]].find(cur_id) == c_[relevant_[i]].end()
	  || c_[relevant_[i]][cur_id] == bv[relevant_[i]]) {
	bs_iter.next();
      } else {
	bs_iter.remove_cur();

	if (candidates_->get_size() == SHORT_CIRCUIT_THRESHOLD) {
	  int vlen;
	  int is_match;
	  unsigned k;
	  for(rset_uint::iterator iter = candidates_->get_iterator();
	      iter.is_cur_valid();
	      iter.next()) {

	      k = iter.get_cur();
	      vlen = s_[k].size();
	      is_match = 1;
	      for(int j = 0; j < vlen; j++) {
		num_steps_++;

		if (bv[s_[k][j].bytenum] != s_[k][j].byteval) {
		  is_match = 0;
		  break;
		}
	      }
	      
	      if (is_match) {
		return k;
	      }	    
	  }

	  return INVALID_ID;
	}
      }
    }
  }

  assert(candidates_->get_size() > SHORT_CIRCUIT_THRESHOLD);
  
  for(rset_uint::iterator iter = candidates_->get_iterator();
      iter.is_cur_valid();
      iter.next()) {
    return iter.get_cur();
  }

  assert(0);
  return INVALID_ID;  
}
