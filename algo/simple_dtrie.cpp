#include "../includes/simple_dtrie.h"
#include "../includes/common.h"
#include "../includes/rset_uint.h"
#include "../includes/uset_uint.h"

#include <stdint.h>
#include <assert.h>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include <iostream>
#include <vector>

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

simple_dtrie::simple_dtrie(const char *cur_time) : rep(cur_time, "simple_dtrie") {
  
}

simple_dtrie::~simple_dtrie() {
  
}

// TODO: probs don't need this
void simple_dtrie::begin_sbv(int id) {
  super::begin_sbv(id);
}

// TODO: probs don't need this
void simple_dtrie::end_sbv(int id) {
  super::end_sbv(id);
}

void simple_dtrie::do_add_byte(int id, unsigned bytenum, unsigned byteval) {
  if (c_.find(bytenum) == c_.end()) {
    byteval_map bs;
    c_[bytenum] = bs;

    id_set ids;
    c_[bytenum][byteval] = ids;
  } else if (c_[bytenum].find(byteval) == c_[bytenum].end()) {
    id_set ids;
    c_[bytenum][byteval] = ids;
  }

  c_[bytenum][byteval].push_back(id);
}

simple_dtrie::c_trie::c_trie(cache *c) {
  cache_ = c;
  u_ = new uset_uint(cache_->size());

  
}

simple_dtrie::c_trie::~c_trie() {
  delete u_;
}

void simple_dtrie::c_trie::cond(unsigned bytenum, uint8_t byteval) {
  u_->begin_trans();

  if (cache_->find(bytenum) != cache_->end()) {
    byteval_map bm = cache_->operator[](bytenum);
    byteval_map::const_iterator b_end = bm.end();
    for(byteval_map::const_iterator b_iter = bm.begin(); b_iter != b_end; b_iter++) {
      if (b_iter->first == byteval) {
	continue;
      }
      
      for(unsigned i = 0; i < b_iter->second.size(); i++) {
	u_->remove(b_iter->second[i]);
      }
    }
  }

  u_->end_trans();
}

void simple_dtrie::c_trie::uncond(void) {
  u_->undo_trans();
}

void simple_dtrie::prepare_to_query() {
  // TODO
}

unsigned simple_dtrie::do_query(uint8_t *bv, unsigned len) {
  // TODO
  return INVALID_ID;
}
