#include "../includes/in_order.h"
#include "../includes/common.h"
#include "../includes/rset_uint.h"

#include <stdint.h>
#include <assert.h>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
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
  /*
  for(unsigned blah = 0; blah < len; blah++) {
    //printf("[%u %u] ", blah, bv[blah]);
  }
  printf("\n");

  for(unsigned blah = 0; blah < s_[7].size(); blah++) {
    //printf("[%u %u] ", s_[7][blah].bytenum, s_[7][blah].byteval);
    if (bv[s_[7][blah].bytenum] != s_[7][blah].byteval) {
      printf("diff [%u %u]\n", s_[7][blah].bytenum, s_[7][blah].byteval);
    }
  }
  printf("\n");
  */

  candidates_->restore();

  unsigned num_matches;
  for(unsigned i = 0; i < num_relevant_; i++) {
    assert(relevant_[i] < len);

    num_steps_++;

    bytepair_set::const_iterator b_end = c_[relevant_[i]].end();
    for(bytepair_set::const_iterator b_iter = c_[relevant_[i]].begin(); 
	b_iter != b_end; b_iter++) {
      num_steps_++;

      if (b_iter->byteval != bv[relevant_[i]]) {
	if (!candidates_->remove(b_iter->id)) {
	  continue;
	}
	
	num_matches = candidates_->get_size();
	if (num_matches == 0) {
	  return INVALID_ID;
	} else if (num_matches == SHORT_CIRCUIT_THRESHOLD) {
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
