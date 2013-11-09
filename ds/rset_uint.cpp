#include "../includes/rset_uint.h"

#include <stdlib.h>
#include <assert.h>

using namespace std;

rset_uint::rset_uint(unsigned size) {
  set_ = (node *) calloc(sizeof(node) * size);
  assert(set_ != NULL);
  cur_vnum_ = 0;
  size_ = size;
}

rset_uint::~rset_uint() {
  assert(set_ != NULL);
  free(set_);
}

void rset_uint::restore() {
  // fail on uint overflow
  assert(cur_vnum_ < cur_vnum_ + 1);
  cur_vnum_++;
}

bool rset_uint::lookup(unsigned, unsigned) {
  return false;
}

bool rset_uint::remove(unsigned, unsigned) {
  return false;
}

rset_uint::iterator::iterator(unsigned) {

}

rset_uint::iterator::~iterator() {

}

bool rset_uint::iterator::has_next() {
  return false;
}

bool rset_uint::iterator::get_cur() {
  return false;
}

void rset_uint::iterator::next() {

}

rset_uint::iterator rset_uint::get_iterator() {
  iterator it(0);
  return it;
}
