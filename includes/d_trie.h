#ifndef _D_TRIE_H
#define _D_TRIE_H

#include "rep.h"
#include <tr1/unordered_map>
#include <stdint.h>

using namespace std;

class d_trie {
 public:
  d_trie(unsigned bytenum, unsigned id);
  ~d_trie(void);
    
  d_trie *decide(uint8_t byteval);
  void extend(uint8_t byteval, unsigned bytenum, unsigned id);
  void add_vect(uint8_t *bv, unsigned len, unsigned id);
  bool is_leaf(void);

  unsigned get_bytenum(void);
  unsigned get_id(void);
  
  void print(void);
  
 protected:
  typedef struct {
    long operator() (const unsigned &k) const {
      return k; 
    }
  } byteval_hash;

  typedef struct {
    bool operator() (const unsigned &x, const unsigned &y) const { 
      return x == y; 
    }
  } byteval_eq;

  typedef tr1::unordered_map<uint8_t, d_trie *, byteval_hash, byteval_eq> offspring;
  offspring children_;
  unsigned bytenum_;
  unsigned id_;
};

#endif
