#ifndef _SIMPLE_DTRIE_H
#define _SIMPLE_DTRIE_H

#include "rep.h"
#include "rset_uint.h"
#include "uset_uint.h"
#include <stdint.h>
#include <iostream>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>

#include <vector>

using namespace std;
using namespace boost;

#pragma once

class simple_dtrie : public virtual rep {
 public:
  simple_dtrie(const char *cur_time);
  ~simple_dtrie(void);

  void do_add_byte(int id, unsigned bytenum, unsigned byteval);
  void prepare_to_query(void);
  unsigned do_query(uint8_t *bv, unsigned len);
  void begin_sbv(int id);
  void end_sbv(int id);

 protected:
  typedef struct {
    long operator() (const uint8_t &k) const {
      return k; 
    }
  } byteval_hash;

  typedef struct {
    bool operator() (const uint8_t &x, const uint8_t &y) const { 
      return x == y; 
    }
  } byteval_eq;

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

  typedef vector<unsigned> id_set;
  typedef unordered_map<uint8_t, id_set, byteval_hash, byteval_eq> byteval_set;
  typedef unordered_map<unsigned, byteval_set, bytenum_hash, bytenum_eq> cache;
  cache c_;

 private:
  typedef rep super;
};

#endif
