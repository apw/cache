#include "../includes/trie_cb.h"
#include "../includes/uset_uint.h"

#include <assert.h>

trie_cb::trie_cb(const char *cur_time)
  : rep(cur_time, "trie_cb"), simple_cb(cur_time) {
  d_ = NULL;
}

trie_cb::~trie_cb() {
  if (d_ != NULL) {
    delete d_;
  }
}

void trie_cb::do_add_byte(int id, unsigned bytenum, unsigned byteval) {
  super::do_add_byte(id, bytenum, byteval);
}

void trie_cb::populate_trie(d_trie *n, uset_uint *candidates, unsigned prev) {
  assert(n != NULL);
  assert(candidates != NULL);
  assert(prev < num_relevant_);
  
  unsigned cur = prev + 1;
  
  // base case?
  
  assert(candidates->get_size() > 0);

  //candidates->print();
  //cout << endl;

  cout << "Q(" << relevant_[prev] << ")" << endl;
  
  for(unsigned i = 0; i < s_.size(); i++) {
    if (candidates->lookup(i)) {      
      if (c_[relevant_[prev]].count(i) > 0) {
	// this is a relevant bytenum
	
	uint8_t byteval = c_[relevant_[prev]][i];
	
	// remove candidates with bytenum 'relevant_[prev]' not set to 'byetval'
	candidates->begin_trans();	
	bytenum_set::const_iterator prev_end = c_[relevant_[prev]].end();
	for(bytenum_set::const_iterator prev_iter = c_[relevant_[prev]].begin();
	    prev_iter != prev_end; prev_iter++) {
	  if (candidates->lookup(prev_iter->first) && prev_iter->second != byteval) {
	    candidates->remove(prev_iter->first);
	    cout << "removing " << prev_iter->first << " b/c byteval " << prev_iter->second << " is not " << ((unsigned)byteval) << endl;
	  }
	}
	candidates->end_trans();
	
	if (candidates->get_size() > 0) {
	  cout << "Cond on " << ((unsigned)byteval) << endl;
	  
	  if (cur == num_relevant_) {
	    n->extend(byteval, INVALID_BYTENUM, i);
	  } else {
	    n->extend(byteval, relevant_[cur], i);
	    d_trie *child = n->decide(byteval);
	    populate_trie(child, candidates, cur);
	  }
	} else {
	  // TODO why do we never hit this assert?
	  assert(0);
	}
	
	candidates->undo_trans();
      } else if (c_[relevant_[prev]].count(i) == 0) {
	// this is an irrelevant bytenum
	
	/*
	if (cur == num_relevant_) {
	  n->extend_x(INVALID_BYTENUM, i);
	} else {
	  n->extend_x(relevant_[cur], i);
	  d_trie *child = n->decide_x();
	  populate_trie(child, candidates, cur);
	}
	*/
	
      }
    }
  }
  
}

void trie_cb::prepare_to_query() {
  super::prepare_to_query();
  
  d_ = new d_trie(relevant_[0], INVALID_ID);
  uset_uint *candidates = new uset_uint(s_.size());
  populate_trie(d_, candidates, 0);
  
  d_->print(); // AHHH
  
  delete candidates;  
}

unsigned trie_cb::do_query_helper(d_trie *current, uint8_t *bv, unsigned len) {
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

unsigned trie_cb::do_query(uint8_t *bv, unsigned len) {
  assert(bv != NULL);
  assert(len > 0);
  
  assert(0); // AHHH
  
  return do_query_helper(d_, bv, len);
}

void trie_cb::begin_sbv(int id) {
  super::begin_sbv(id);  
}

void trie_cb::end_sbv(int id) {
  super::end_sbv(id);
}

