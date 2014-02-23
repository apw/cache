#include "../includes/rep.h"
#include "../includes/in_order.h"
#include "../includes/carebear_forest.h"
#include "../includes/common.h"

#include <iostream>
#include <vector>
#include <algorithm>

#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

carebear_forest::carebear_forest(const char *cur_time) 
  : rep(cur_time, "carebear_forest"), in_order(cur_time) {
  
}

carebear_forest::~carebear_forest() {
  for(unsigned i = 0; i < forest.size(); i++) {
    delete forest[i];
  }
}

int carebear_forest::get_max_bytenum(uset_uint *done, uset_uint *u) {
  unsigned max_num_care = 0;
  int max_bytenum = -1;
  cache::const_iterator c_end = c_.end();
  for(cache::const_iterator c_iter = c_.begin(); c_iter != c_end; c_iter++) {
    unsigned cur_num_care = 0;
    bytenum_set::const_iterator b_end = c_[c_iter->first].end();
    for(bytenum_set::const_iterator b_iter = c_[c_iter->first].begin(); 
	b_iter != b_end; b_iter++) {
      if (done->lookup(b_iter->first) && u->lookup(b_iter->first)) {
	cur_num_care++;
      }
    }
    
    if (cur_num_care > max_num_care) {
      max_num_care = cur_num_care;
      max_bytenum = c_iter->first;
    }
  }
  
  return max_bytenum;
}

void carebear_forest::populate_subtrie(d_trie *d, uset_uint *done, uset_uint *u) {
  // remove all vectors that have a don't-care at d->get_bytenum() from u
  unsigned num_left = 0;
  unsigned last;
  
  u->begin_trans();
  for(unsigned i = 0, n = done->get_capacity(); i < n; i++) {
    if (done->lookup(i) && u->lookup(i) 
	&& c_[d->get_bytenum()].count(i) == 0) {
      u->remove(i);
    } else if (done->lookup(i) && u->lookup(i) 
	       && c_[d->get_bytenum()].count(i) > 0) {
      num_left++;
      last = i;
    }
  }
  u->end_trans();

  // base case if only one vector is left here!!
  if (num_left == 1) {
    assert(c_[d->get_bytenum()].count(last) > 0);
    
    d->extend(c_[d->get_bytenum()][last], INVALID_BYTENUM, last);
    u->undo_trans();
    
    done->remove(last);
    return;
  }
  
  // Beginning of stupid Theta(n^2)
  // of all remaining candidate vectors, find all unique vals at d->get_bytenum()
  tr1::unordered_set<uint8_t> val_set;
  bytenum_set::const_iterator b_end = c_[d->get_bytenum()].end();
  for(bytenum_set::const_iterator b_iter = c_[d->get_bytenum()].begin(); 
      b_iter != b_end; b_iter++) {
    // first is id; second is byteval
    if (done->lookup(b_iter->first) && u->lookup(b_iter->first) 
	&& val_set.count(b_iter->second) == 0) {
      val_set.insert(b_iter->second);      
    }
  }

  for (tr1::unordered_set<uint8_t>::const_iterator v_iter = val_set.begin();
       v_iter != val_set.end(); v_iter++) {
    // remove all vectors that don't have the current byteval at the bytenum
    u->begin_trans();
    bytenum_set::const_iterator b_end = c_[d->get_bytenum()].end();
    for(bytenum_set::const_iterator b_iter = c_[d->get_bytenum()].begin(); 
	b_iter != b_end; b_iter++) {
      if (done->lookup(b_iter->first) && u->lookup(b_iter->first)
	  && b_iter->second != *v_iter) {
	u->remove(b_iter->first);
      }
    }
    u->end_trans();
    
    // find next bytenum with highest utility
    int max_bytenum = get_max_bytenum(done, u);
    if (max_bytenum == -1) {
      assert(0);
    }
    
    // extend the trie!
    d->extend(*v_iter, max_bytenum, INVALID_BYTENUM);
    d_trie *child = d->decide(*v_iter);
    
    // ...and now recurse down!
    populate_subtrie(child, done, u);
    
    // undo trans    
    u->undo_trans();
  }
  
  // End of stupid Theta(n^2)

  // undo removal
  u->undo_trans();
}

void carebear_forest::prepare_to_query() {
  uset_uint *done = new uset_uint(current_id_ + 1);
  uset_uint *u = new uset_uint(current_id_ + 1);
  
  while(done->get_size() > 0) {
    // find the bytenum with the largest number of don't care
    assert(u->get_size() == u->get_capacity());
    int max_bytenum = get_max_bytenum(done, u);
    if (max_bytenum == -1) {
      assert(0);
    }
    
    // make the max_bytenum the question at the top of the subtrie
    d_trie *d = new d_trie(max_bytenum, INVALID_ID);
    forest.push_back(d);
  
    populate_subtrie(d, done, u);
  }
  
  delete u;
  delete done;
}

unsigned carebear_forest::do_query(uint8_t *bv, unsigned len) {
  // QUERY!
  
  assert(0);
  return INVALID_ID;  
}
