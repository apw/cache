#ifndef _DGEN_H
#define _DGEN_H

#include "d_trie.h"

#include <tr1/unordered_map>
#include <vector>

using namespace std;

typedef struct {
  long operator() (const unsigned &k) const {
    return k; 
  }
} id_hash;

typedef struct {
  bool operator() (const unsigned &x, const unsigned &y) const { 
    return x == y; 
  }
} id_eq;

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

// vectyl
typedef tr1::unordered_map<unsigned, unsigned, id_hash, id_eq> bytenum_set;
// tera
typedef tr1::unordered_map<unsigned, bytenum_set, bytenum_hash, bytenum_eq> cache;


typedef struct {
  unsigned bytenum;
  uint8_t byteval;
} entry;

typedef vector<entry> vect;


struct cache_params {
  unsigned num_vects;
  unsigned vect_len;
  float m_num_rel;
  float std_num_rel;
};


void gen_opt_dtrie(struct cache_params *, d_trie *, cache *);

#endif
