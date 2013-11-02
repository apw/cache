#ifndef _IN_ORDER_H
#define _IN_ORDER_H

#include "rep.h"
#include <stdint.h>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>

using namespace std;
using namespace boost;

#pragma once

#define IN_ORDER_OUTFNAME "in_order"

class in_order : public virtual rep {
 public:
  in_order(const char *cur_time);
  ~in_order(void);

  void do_add_byte(int id, unsigned bytenum, unsigned byteval);

  int do_query(uint8_t *bv, unsigned len);

 private:

  class bytepair {
  public:
    int id;
    uint8_t byteval;
    
    bool operator==(const bytepair &b) const {
      return id == b.id && byteval == b.byteval;
    }
  };

  struct hash_func {
    size_t operator()(const bytepair &b) const {
      return b.id;
    }
  };

  typedef unordered_set<bytepair, hash_func> bytepair_set;
  typedef unordered_map<unsigned, bytepair_set> cache;
  cache c_;
};

#endif
