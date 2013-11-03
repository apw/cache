#ifndef _IN_ORDER_H
#define _IN_ORDER_H

#include "rep.h"
#include <stdint.h>
#include <iostream>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>

#include <vector>

using namespace std;
using namespace boost;

#pragma once

#define SHORT_CIRCUIT_THRESHOLD 1

class in_order : public virtual rep {
 public:
  in_order(const char *cur_time);
  ~in_order(void);

  void do_add_byte(int id, unsigned bytenum, unsigned byteval);
  void prepare_to_query(void);
  int do_query(uint8_t *bv, unsigned len);
  void begin_sbv(int id);
  void end_sbv(int id);

 protected:
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

  typedef struct {
    long operator() (const unsigned &k) const {
      return k; 
    }
  } bytenum_hash;

  typedef struct {
    bool operator() (const unsigned &x, const unsigned &y) const { 
      return x == y; 
    }
  } bytenum_eq;

  typedef unordered_set<bytepair, hash_func> bytepair_set;
  typedef unordered_map<unsigned, bytepair_set, bytenum_hash, bytenum_eq> cache;
  cache c_;

  unsigned num_relevant_;
  unsigned *relevant_;

  class numval {
  public:
    unsigned bytenum;
    uint8_t byteval;
  };

  typedef vector<numval> vect;
  typedef vector<vect> store;
  store s_;

 private:
  typedef rep super;
};

#endif
