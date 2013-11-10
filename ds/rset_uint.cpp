#include "../includes/rset_uint.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

using namespace std;

#define INVALID_P ((unsigned) -1)

rset_uint::rset_uint(unsigned capacity) {
  assert(capacity < INVALID_P);
  set_ = (node *) calloc(capacity, sizeof(node));
  assert(set_ != NULL);
  cur_vnum_ = 1;
  capacity_ = capacity;
  size_ = capacity;
  first_ = 0;
  set_[capacity - 1].next = INVALID_P;
  set_[0].prev = INVALID_P;
}

rset_uint::~rset_uint() {
  assert(set_ != NULL);
  free(set_);
}

bool rset_uint::is_valid(unsigned n) {
  return n < capacity_;
}

void rset_uint::restore() {
  // fail on uint overflow
  assert(cur_vnum_ < cur_vnum_ + 1);
  cur_vnum_++;
  first_ = 0;
  size_ = capacity_;
}

bool rset_uint::lookup(unsigned n) {
  assert(n < capacity_);
  return set_[n].is_present < cur_vnum_;
}

unsigned rset_uint::get_size() {
  return size_;
}

unsigned rset_uint::get_capacity() {
  return capacity_;
}

unsigned rset_uint::get_next(unsigned n) {
  assert(lookup(n));
  unsigned next = (set_[n].n_vnum == cur_vnum_) ? set_[n].next : n + 1;
  return (!is_valid(next)) ? INVALID_P : next;
}

unsigned rset_uint::get_prev(unsigned n) {
  assert(lookup(n));
  return (set_[n].p_vnum == cur_vnum_) ? set_[n].prev : n - 1;
}

bool rset_uint::remove(unsigned n) {
  assert(n < capacity_);

  bool was_there = lookup(n);
  if (was_there) {
    size_--;
    assert(size_ < capacity_);

    if (first_ == n) {
      first_ = get_next(n);
    }

    unsigned prev = get_prev(n);
    unsigned next = get_next(n);
    if (is_valid(prev)) {
      if (is_valid(next)) {
	set_[prev].next = next;

	set_[next].prev = prev;
	set_[next].p_vnum = cur_vnum_;
      } else {
	set_[prev].next = INVALID_P;
      }

      set_[prev].n_vnum = cur_vnum_;
    } else if (is_valid(next)) {
      set_[next].prev = INVALID_P;
      set_[next].p_vnum = cur_vnum_;
    }

    set_[n].is_present = cur_vnum_;
  }

  return was_there;
}

rset_uint::iterator::iterator(rset_uint *rset) {
  iterator::rset_ = rset;
  iterator::assigned_vnum_ = rset->cur_vnum_;
  iterator::cur_node_ = rset->first_;
}

rset_uint::iterator::~iterator() {
  
}

bool rset_uint::iterator::is_cur_valid() {
  return rset_->is_valid(iterator::cur_node_);
}

unsigned rset_uint::iterator::get_cur() {
  return iterator::cur_node_;
}

void rset_uint::iterator::next() {
  iterator::cur_node_ = rset_->get_next(iterator::cur_node_);
}

rset_uint::iterator rset_uint::get_iterator() {
  iterator it(this);
  return it;
}
