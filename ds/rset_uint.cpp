#include "../includes/rset_uint.h"

using namespace std;

rset_uint::rset_uint(unsigned) {

}

rset_uint::~rset_uint(void) {

}

void rset_uint::restore(void) {

}

bool rset_uint::lookup(unsigned, unsigned) {
  return false;
}

bool rset_uint::remove(unsigned, unsigned) {
  return false;
}

rset_uint::iterator::iterator(unsigned) {

}

rset_uint::iterator::~iterator(void) {

}

bool rset_uint::iterator::has_next(void) {
  return false;
}

bool rset_uint::iterator::get_cur(void) {
  return false;
}

void rset_uint::iterator::next(void) {

}

rset_uint::iterator rset_uint::get_iterator(void) {
  iterator it(0);
  return it;
}
