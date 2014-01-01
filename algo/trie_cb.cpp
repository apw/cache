#include "../includes/trie_cb.h"

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

void trie_cb::prepare_to_query() {
  super::prepare_to_query();
  
  d_ = new d_trie(relevant_[0], INVALID_ID);
  d_trie *current_;
  
  unsigned num_vects = s_.size();
  for(unsigned i = 0; i < num_vects; i++) {
    current_ = d_;
    for(unsigned j = 0; j < num_relevant_; j++) {
      assert(c_.count(relevant_[j]) == 1);
      if (c_[relevant_[j]].count(i) > 0) {
	unsigned next = j + 1;
	for(; next < num_relevant_; next++) {
	  if (c_[relevant_[next]].count(i) > 0) {
	    break;
	  }
	}

	assert(c_[relevant_[j]].count(i) == 1);
	if (next < num_relevant_) {
	  current_->extend(c_[relevant_[j]][i], relevant_[next], i);
	  current_ = current_->decide(c_[relevant_[j]][i]);
	  assert(current_ != NULL);
	} else {
	  current_->extend(c_[relevant_[j]][i], INVALID_BYTENUM, i);
	}
      }
    }
  }
}

unsigned trie_cb::do_query(uint8_t *bv, unsigned len) {
  d_trie *current_ = d_;
  
  while(!current_->is_leaf()) {
    num_steps_++;

    current_ = current_->decide(bv[current_->get_bytenum()]);
    if (current_ == NULL) {
      return INVALID_ID;
    }
  }

  assert(current_->get_bytenum() == INVALID_BYTENUM);
  return current_->get_id();
}

void trie_cb::begin_sbv(int id) {
  super::begin_sbv(id);  
}

void trie_cb::end_sbv(int id) {
  super::end_sbv(id);
}

