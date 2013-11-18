#include "../includes/simple_dtrie.h"
#include "../includes/common.h"
#include "../includes/rset_uint.h"
#include "../includes/uset_uint.h"

#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include <iostream>
#include <vector>

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
    byteval_map bm;
    c_[bytenum] = bm;
  }

  if (c_[bytenum].find(byteval) == c_[bytenum].end()) {
    id_set ids;
    c_[bytenum][byteval] = ids;
  }

  c_[bytenum][byteval].push_back(id);
}

simple_dtrie::c_trie::c_trie() {

}

simple_dtrie::c_trie::~c_trie() {
  delete u_;
}

void simple_dtrie::c_trie::load_cache(cache *c) {
  cache_ = c;
  u_ = new uset_uint(cache_->size());
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

void simple_dtrie::c_trie::uncond() {
  u_->undo_trans();
}

float simple_dtrie::c_trie::get_prop(unsigned bytenum, uint8_t byteval) {
  if (cache_->find(bytenum) == cache_->end()) {
    return 0.0;
  }

  byteval_map bm = cache_->operator[](bytenum);
  if (bm.find(byteval) == bm.end()) {
    return 0.0;
  }

  unsigned n = 0, b = 0;
  cache::const_iterator c_end = cache_->end();
  for(cache::const_iterator c_iter = cache_->begin(); c_iter != c_end; c_iter++) {
    byteval_map::const_iterator b_end = bm.end();
    for(byteval_map::const_iterator b_iter = bm.begin(); 
	b_iter != b_end; b_iter++) {
      if (b_iter->first == byteval) {
	for(unsigned i = 0; i < b_iter->second.size(); i++) {
	  n += u_->lookup(b_iter->second[i]) ? b_iter->second[i] : 0;
	}
      } else {
	for(unsigned i = 0; i < b_iter->second.size(); i++) {
	  b += u_->lookup(b_iter->second[i]) ? b_iter->second[i] : 0;
	}
      }
    }    
  }


  return n / (n + b);
}

simple_dtrie::q_trie::q_trie() {
  
}

simple_dtrie::q_trie::~q_trie() {
  
}

void simple_dtrie::q_trie::update(uint8_t *bv, unsigned len) {
  for(unsigned i = 0; i < len; i++) {
    q_[i].denom++;
    if (q_[i].pm.find(bv[i]) == q_[i].pm.end()) {
      q_[i].pm[bv[i]] = 1;
    } else {
      q_[i].pm[bv[i]]++;
    }
  }
}

void simple_dtrie::q_trie::cond(unsigned bytenum, uint8_t byteval) {
  
}

void simple_dtrie::q_trie::uncond() {
  
}

float simple_dtrie::q_trie::get_prop(unsigned bytenum, uint8_t byteval) {
  if (q_.find(bytenum) == q_.end()
      || q_[bytenum].pm.find(byteval) == q_[bytenum].pm.end()) {
    return 0.0;
  }

  return q_[bytenum].pm[byteval] / q_[bytenum].denom;
}

unsigned simple_dtrie::get_highest_utility_bytenum() {
  unsigned max_bytenum = INVALID_BYTENUM;
  unsigned bytenum;
  float max_u = 0.0;
  float u;
  for(c_trie::iterator iter = cond_cache_.get_iterator(); 
      iter.is_cur_valid(); iter.next()) {
    bytenum = iter.get_cur();
    u = get_cond_utility(bytenum);
    if (max_u < u) {
      max_u = u;
      max_bytenum = bytenum;
    }
  }
  
  return max_bytenum;
}

simple_dtrie::c_trie::iterator simple_dtrie::c_trie::get_iterator() {
  simple_dtrie::c_trie::iterator it(this);
  return it;
}

simple_dtrie::c_trie::iterator::iterator(c_trie *ct) : u_iter_(ct->u_) {
  
}

simple_dtrie::c_trie::iterator::~iterator() {

}

bool simple_dtrie::c_trie::iterator::is_cur_valid() {
  return u_iter_.is_cur_valid();
}

unsigned simple_dtrie::c_trie::iterator::get_cur() {
  return u_iter_.get_cur();
}

void simple_dtrie::c_trie::iterator::next() {
  return u_iter_.next();
}


float simple_dtrie::get_cond_utility(unsigned bytenum) {
  if (c_.find(bytenum) == c_.end()) {
    return 0.0;
  }

  float acc = 0.0;
  byteval_map::const_iterator b_end = c_[bytenum].end();
  for(byteval_map::const_iterator b_iter = c_[bytenum].begin(); 
      b_iter != b_end; b_iter++) {
    acc += cond_query_.get_prop(bytenum, b_iter->first)
      * (1 - cond_cache_.get_prop(bytenum, b_iter->first));    
  }
  
  return acc;
}

void simple_dtrie::prepare_to_query() {
  cond_cache_.load_cache(&c_);
}

unsigned simple_dtrie::do_query(uint8_t *bv, unsigned len) {
  // TODO
  return INVALID_ID;
}
