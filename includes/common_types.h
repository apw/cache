#ifndef _COMMON_TYPES_H
#define _COMMON_TYPES_H

#include <vector>
#include <tr1/unordered_map>
#include <tr1/unordered_set>

#include <stdint.h>

/* Teravectyl */
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

typedef tr1::unordered_map<unsigned, unsigned, id_hash, id_eq> bytenum_set;
typedef tr1::unordered_map<unsigned, bytenum_set, bytenum_hash, bytenum_eq> cache;

/* store */
class numval {
 public:
  unsigned bytenum;
  uint8_t byteval;
};

typedef vector<numval> vect;
typedef vector<vect> store;

typedef struct {
  vect ve;
  unsigned id;
} c_entry;


#endif
