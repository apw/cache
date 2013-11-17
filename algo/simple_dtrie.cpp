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
    byteval_set bs;
    c_[bytenum] = bs;

    id_set ids;
    c_[bytenum][byteval] = ids;
  } else if (c_[bytenum].find(byteval) == c_[bytenum].end()) {
    id_set ids;
    c_[bytenum][byteval] = ids;
  }

  c_[bytenum][byteval].push_back(id);
}

void simple_dtrie::prepare_to_query() {
  // TODO
}

unsigned simple_dtrie::do_query(uint8_t *bv, unsigned len) {
  // TODO
  return INVALID_ID;
}
