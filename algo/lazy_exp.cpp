#include "../includes/lazy_exp.h"
#include "../includes/common.h"

#include <assert.h>

lazy_exp::lazy_exp(const char *cur_time)
  : rep(cur_time, "lazy_exp"), simple_cb(cur_time) {
  z_ = NULL;
}

lazy_exp::~lazy_exp() {
  if (z_ != NULL) {
    delete z_;
  }
}

void lazy_exp::do_add_byte(int id, unsigned bytenum, unsigned byteval) {
  super::do_add_byte(id, bytenum, byteval);
}

void lazy_exp::prepare_to_query() {
  super::prepare_to_query();
  
  z_ = new lazy_trie(INVALID_ID, 0, num_relevant_, relevant_);
  for(unsigned i = 0; i < s_.size(); i++) {
    c_entry ce;
    ce.ve = s_[i];
    ce.id = i;
    z_->add_vect(ce);
  }
}

unsigned lazy_exp::do_query(uint8_t *bv, unsigned len) {
  // TODO COUNT QUESTIONS!!!!!!
  // TODO use carebear comparison function first maybe?
  lazy_trie *cur = z_;
  
  do {
    if (cur->is_leaf()) {
      assert(cur->get_bytenum() == INVALID_BYTENUM);
      return cur->get_id();
    }
    
    uint8_t byteval = bv[cur->get_bytenum()];
    cur = cur->decide(byteval);
  } while (cur != NULL);
  
  return INVALID_ID;
}

void lazy_exp::begin_sbv(int id) {
  super::begin_sbv(id);  
}

void lazy_exp::end_sbv(int id) {
  super::end_sbv(id);
}

