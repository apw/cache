#include "../includes/uset_uint.h"

#include <vector>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

using namespace std;

uset_uint::uset_uint(unsigned capacity) {
  assert(capacity > 0);
  capacity_ = capacity;
  size_ = capacity;
  
  set_ = (unsigned *) calloc(capacity, sizeof(unsigned));
  assert(set_ != NULL);
  // TODO pit pack stuff :D
}

uset_uint::~uset_uint() {
  free(set_);
}

bool uset_uint::lookup(unsigned n) {
  return set_[n] == 0;
}

unsigned uset_uint::get_size() {
  return size_;
}

unsigned uset_uint::get_capacity() {
  return capacity_;
}

bool uset_uint::remove(unsigned n) {
  remove_set rs = u_.back();
  rs.push_back(n);
  
  unsigned was_there = set_[n];
  set_[n]++;
  return was_there;
}

void uset_uint::begin_trans() {
  remove_set rs;
  u_.push_back(rs);
}

void uset_uint::end_trans() {
  
}

void uset_uint::undo_trans() {  
  remove_set rs = u_.back();
  unsigned n;
  while(rs.size() > 0) {
    n = rs.back();
    rs.pop_back();
    assert(set_[n] > 0);
    set_[n]--;
  }

  u_.pop_back();
}


uset_uint::iterator::iterator(uset_uint *uset) {
  /*
  iterator::uset_ = uset;
  iterator::assigned_vnum_ = uset->cur_vnum_;
  iterator::cur_node_ = uset->first_;
  */
}

uset_uint::iterator::~iterator() {
  
}

bool uset_uint::iterator::is_cur_valid() {
  return false;
}

unsigned uset_uint::iterator::get_cur() {
  return 0;
}

void uset_uint::iterator::next() {
  //iterator::cur_node_ = uset_->get_next(iterator::cur_node_);
}

void uset_uint::iterator::remove_cur() {
  /*
  unsigned tmp = iterator::cur_node_;
  iterator::cur_node_ = uset_->get_next(iterator::cur_node_);
  uset_->remove(tmp);
  */
}

uset_uint::iterator uset_uint::get_iterator() {
  iterator it(this);
  return it;
}

