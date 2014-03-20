#include "../includes/rep.h"
#include "../includes/in_order.h"
#include "../includes/greedy_trie.h"
#include "../includes/common.h"

#include <iostream>
#include <vector>
#include <algorithm>

#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

greedy_trie::greedy_trie(const char *cur_time) 
  : rep(cur_time, "greedy_trie"), in_order(cur_time) {
  d_ = NULL;
}

greedy_trie::~greedy_trie() {
  if (d_ != NULL) {
    delete d_;
  }
}

int greedy_trie::get_max_bytenum(uset_uint *done, uset_uint *u, uset_uint *bytenums_left) {
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

void greedy_trie::populate_subtrie(d_trie *d, uset_uint *done, uset_uint *u,
				   uset_uint *bytenums_left) {
  // X-PATH CONSTRUCTION
  // remove all vectors that don't have a don't-care at d->get_bytenum() from u
  u->begin_trans();
  for(unsigned i = 0, n = done->get_capacity(); i < n; i++) {
    if (done->lookup(i) && u->lookup(i) 
	&& c_[d->get_bytenum()].count(i) > 0) {
      u->remove(i);
    }
  }
  u->end_trans();
  
  int max_bytenum = get_max_bytenum(done, u, bytenums_left);
  if (max_bytenum == -1) {
    for(unsigned i = 0, n = done->get_capacity(); i < n; i++) {
      if (done->lookup(i) && u->lookup(i)) {
	// 'i' is the id of the vector that we are about to finish inserting
	d->extend_x(INVALID_BYTENUM, i);
	done->remove(i);
      }
    }
  } else {    
    // extend the trie!
    d->extend_x(max_bytenum, INVALID_ID);
    d_trie *child = d->decide_x();
    
    // ...and now recurse down!
    populate_subtrie(child, done, u, bytenums_left);
    
    // undo trans performed in get_max_bytenum
    bytenums_left->undo_trans();
  }
  
  // undo trans made for the x-path construction
  // (new slate for carebear-path construction)
  u->undo_trans();
  
  // CAREBEAR-PATH CONSTRUCTION
  // remove all vectors that do have a don't-care at d->get_bytenum() from u
  u->begin_trans();
  for(unsigned i = 0, n = done->get_capacity(); i < n; i++) {
    if (done->lookup(i) && u->lookup(i) 
	&& c_[d->get_bytenum()].count(i) == 0) {
      u->remove(i);
    }
  }
  u->end_trans();

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
    int max_bytenum = get_max_bytenum(done, u, bytenums_left);
    if (max_bytenum == -1) {
      for(unsigned i = 0, n = done->get_capacity(); i < n; i++) {
	if (done->lookup(i) && u->lookup(i)) {
	  // 'i' is the id of the vector that we are about to finish inserting
	  d->extend(c_[d->get_bytenum()][i], INVALID_BYTENUM, i);
	  done->remove(i);
	}
      }
    } else {    
      // extend the trie!
      d->extend(*v_iter, max_bytenum, INVALID_ID);
      d_trie *child = d->decide(*v_iter);
      
      // ...and now recurse down!
      populate_subtrie(child, done, u, bytenums_left);
      
      // undo trans performed in get_max_bytenum
      bytenums_left->undo_trans();
    }
    
    // undo trans
    u->undo_trans();    
  }
  
  // End of stupid Theta(n^2)

  // undo removal
  u->undo_trans();
}

void greedy_trie::prepare_to_query() {
  // find max relevant bytenum in tera
  max_relevant_bytenum_ = 0;
  cache::const_iterator c_end = c_.end();
  for(cache::const_iterator c_iter = c_.begin(); c_iter != c_end; c_iter++) {
    if (c_iter->first > max_relevant_bytenum_) {
      max_relevant_bytenum_ = c_iter->first;
    }
  }

  uset_uint *done = new uset_uint(current_id_ + 1);
  uset_uint *u = new uset_uint(current_id_ + 1);
  uset_uint *bytenums_left = new uset_uint(max_relevant_bytenum_ + 1);
  
  done->begin_trans();
  
  // find the bytenum with the largest number of don't care
  int max_bytenum = get_max_bytenum(done, u, bytenums_left);
  assert(max_bytenum != -1);
  
  // make the max_bytenum the question at the top of the subtrie
  d_ = new d_trie(max_bytenum, INVALID_ID);
  
  populate_subtrie(d_, done, u, bytenums_left);
  
  // undo trans made by get_max_bytenum
  bytenums_left->undo_trans();
  
  done->end_trans();
  
  delete u;
  delete done;
  delete bytenums_left;
}

unsigned greedy_trie::do_query_helper(d_trie *current, uint8_t *bv, unsigned len) {
  num_steps_++;
  if (current->is_leaf() && !current->x_exists()) {
    assert(current->get_bytenum() == INVALID_BYTENUM);
    return current->get_id();
  }
  
  bool x = false;
  d_trie *child = current->decide(bv[current->get_bytenum()]);
  if (child == NULL) {
    if (current->x_exists()) {
      child = current->decide_x();
      if (child == NULL) {
	return INVALID_ID;
      }
      
      x = true;
    } else {
      return INVALID_ID;
    }
  }
  
  unsigned res = do_query_helper(child, bv, len);
  if (res == INVALID_ID) {
    if (!x && current->x_exists()) {
      child = current->decide_x();
      if (child == NULL) {
	return INVALID_ID;
      }
      
      res = do_query_helper(child, bv, len);
      return res;
    }
  }
  
  return res;
}

unsigned greedy_trie::do_query(uint8_t *bv, unsigned len) {
  assert(bv != NULL);
  assert(len > 0);
  
  return do_query_helper(d_, bv, len);
}

void greedy_trie::viz() {
  char *dot_filename = (char *) calloc(BUFLEN, sizeof(char));
  assert(dot_filename);
  
  snprintf(dot_filename, sizeof(char) * BUFLEN, "viz/%s.dot", outfile_basename_);
  d_->gen_graph(dot_filename);
  
  free(dot_filename);
}
