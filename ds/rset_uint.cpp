#include "../includes/rset_uint.h"

#include <stdlib.h>
#include <assert.h>

using namespace std;

#define INVALID_P ((unsigned) -1)
#define IS_VALID(var) (var < size_)

rset_uint::rset_uint(unsigned size) {
  assert(size < INVALID_P);
  set_ = (node *) calloc(size, sizeof(node));
  assert(set_ != NULL);
  cur_vnum_ = 1;
  size_ = size;
  first_ = 0;
  set_[size - 1].next = INVALID_P;
  set_[0].prev = INVALID_P;
}

rset_uint::~rset_uint() {
  assert(set_ != NULL);
  free(set_);
}

void rset_uint::restore() {
  // fail on uint overflow
  assert(cur_vnum_ < cur_vnum_ + 1);
  cur_vnum_++;
  first_ = 0;
}

bool rset_uint::lookup(unsigned n) {
  return set_[n].is_present < cur_vnum_;
}

unsigned rset_uint::get_next(unsigned n) {
  assert(lookup(n));
  unsigned next = (set_[n].n_vnum == cur_vnum_) ? set_[n].next : n + 1;
  return next >= size_ ? INVALID_P : next;
}

unsigned rset_uint::get_prev(unsigned n) {
  assert(lookup(n));
  return (set_[n].n_vnum == cur_vnum_) ? set_[n].prev : n - 1;
}

bool rset_uint::remove(unsigned n) {
  bool was_there = lookup(n);
  if (was_there) {
    if (first_ == n) {
      first_ = get_next(n);
    }

    unsigned prev = get_prev(n);
    unsigned next = get_next(n);
    if (IS_VALID(prev)) {
      if (IS_VALID(next)) {
	set_[prev].next = next;

	set_[next].prev = prev;
	set_[next].p_vnum = cur_vnum_;
      } else {
	set_[prev].next = INVALID_P;
      }

      set_[prev].n_vnum = cur_vnum_;
    } else if (IS_VALID(next)) {
      set_[next].prev = INVALID_P;
      set_[next].p_vnum = cur_vnum_;
    }

    set_[n].is_present = cur_vnum_;
  }

  return was_there;
}

rset_uint::iterator::iterator(unsigned v_num, unsigned first) {
  iterator::assigned_vnum_ = v_num;
  iterator::cur_node_ = first;
}

rset_uint::iterator::~iterator() {
  
}

bool rset_uint::iterator::has_next() {
  //return IS_VALID(set_[iterator::cur_node_].next);
  return false;
}

bool rset_uint::iterator::get_cur() {
  return false;
}

void rset_uint::iterator::next() {

}

rset_uint::iterator rset_uint::get_iterator() {
  iterator it(cur_vnum_, first_);
  return it;
}
