#include "../includes/trie_cb.h"
#include "../includes/uset_uint.h"
#include "../includes/common.h"

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

void trie_cb::populate_trie(d_trie *n, uset_uint *candidates,
			    uset_uint *done, unsigned prev) {
  assert(n != NULL);
  assert(candidates != NULL);
  assert(prev < num_relevant_);
  
  unsigned cur = prev + 1;
  
  assert(candidates->get_size() > 0);

  for(unsigned i = 0; i < s_.size(); i++) {
    if (candidates->lookup(i) && done->lookup(i)) {    
      if (c_[relevant_[prev]].count(i) > 0) {
	// this is a relevant bytenum
	
	uint8_t byteval = c_[relevant_[prev]][i];
	
	// remove candidates with bytenum 'relevant_[prev]' not set to 'byteval'
	// or candidates with bytenum 'relevant_[prev]' as irrelevant
	candidates->begin_trans();
	for(unsigned j = 0; j < s_.size(); j++) {
	  if (candidates->lookup(j) && (c_[relevant_[prev]].count(j) == 0
					|| c_[relevant_[prev]][j] != byteval)) {
	    candidates->remove(j);
	  }
	}
	candidates->end_trans();
	
	assert(candidates->get_size() >= 1);
	
	if (cur == num_relevant_) {
	  n->extend(byteval, INVALID_BYTENUM, i);
	  
	  // premenantly remove from the done set
	  for(unsigned j = 0; j < s_.size(); j++) {
	    if (candidates->lookup(j)) {
	      done->remove(j);
	    }
	  }
	} else {
	  n->extend(byteval, relevant_[cur], INVALID_ID);
	  d_trie *child = n->decide(byteval);
	  populate_trie(child, candidates, done, cur);
	}
	
	candidates->undo_trans();
      } else if (c_[relevant_[prev]].count(i) == 0) {
	// this is an irrelevant bytenum
	
	// TODO what if rest of bytenums for this vector are irrelevant?
	if (cur == num_relevant_) {
	  n->extend_x(INVALID_BYTENUM, i);
	  
	  // premenantly remove from the done set
	  for(unsigned j = 0; j < s_.size(); j++) {
	    if (candidates->lookup(j)) {
	      done->remove(j);
	    }
	  }
	} else {
	  // remove candidates that have relevant_[prev] as a relevant bytenum
	  candidates->begin_trans();
	  for(unsigned j = 0; j < s_.size(); j++) {
	    if (candidates->lookup(j) && c_[relevant_[prev]].count(j) > 0) {
	      candidates->remove(j);
	    }
	  }
	  candidates->end_trans();
	  
	  assert(candidates->get_size() > 0);
	  
	  n->extend_x(relevant_[cur], i);
	  d_trie *child = n->decide_x();
	  populate_trie(child, candidates, done, cur);
	  
	  candidates->undo_trans();
	}
		
      } else {
	assert(0);
      }
      
    }
  }
  
}

void trie_cb::prepare_to_query() {
  super::prepare_to_query();
  
  d_ = new d_trie(relevant_[0], INVALID_ID);
  uset_uint *candidates = new uset_uint(s_.size());
  uset_uint *done = new uset_uint(s_.size());
  done->begin_trans();
  populate_trie(d_, candidates, done, 0);
  done->end_trans();
  
  //d_->print(); // AHHH
  
  delete candidates;  
  delete done;
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
  
  //assert(0); // AHHH
  
  return do_query_helper(d_, bv, len);
}

void trie_cb::begin_sbv(int id) {
  super::begin_sbv(id);  
}

void trie_cb::end_sbv(int id) {
  super::end_sbv(id);
}

