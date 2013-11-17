#ifndef _SIMPLE_DTRIE_H
#define _SIMPLE_DTRIE_H

#include "rep.h"
#include "rset_uint.h"
#include "uset_uint.h"
#include "common.h"

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
  typedef unordered_map<uint8_t, id_set, byteval_hash, byteval_eq> byteval_map;
  typedef unordered_map<unsigned, byteval_map, bytenum_hash, bytenum_eq> cache;
  cache c_;

  class c_trie {
  public:
    c_trie(cache *);
    ~c_trie(void);

    void cond(unsigned bytenum, uint8_t byteval);
    void uncond(void);

    /*
     * TODO use some iterator to get current row of bytenums
     */    
  private:
    uset_uint *u_;
    cache *cache_;
  };

  class q_trie {
  public:
    q_trie(void);
    ~q_trie(void);

    void update(uint8_t *bv, unsigned len);
    void cond(unsigned bytenum, uint8_t byteval);
    void uncond(void);

  private:
    typedef unordered_map<uint8_t, unsigned> prop_map;
    typedef struct {
      unsigned denom;
      prop_map pm;
    } bytenum_md;

    typedef unordered_map<unsigned, bytenum_md> query_md;

    query_md q_;
  };

 private:
  typedef rep super;
};

#endif
