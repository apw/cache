#ifndef _LAZY_TRIE_H
#define _LAZY_TRIE_H

#include "rep.h"
#include "common_types.h"
#include <tr1/unordered_map>
#include <vector>
#include <stdint.h>

using namespace std;

class lazy_trie {
 public:
  lazy_trie(unsigned bytenum, unsigned id);
  ~lazy_trie(void);
    
  lazy_trie *decide(uint8_t byteval);
  void extend(uint8_t byteval, unsigned bytenum, unsigned id);
  void add_vect(vect *bv, unsigned id);
  bool is_leaf(void);
  
  unsigned get_bytenum(void);
  unsigned get_id(void);
  
  void print(void);
  
 protected:
  void print_helper(unsigned);
  
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
  
  typedef tr1::unordered_map<uint8_t, lazy_trie *, byteval_hash, byteval_eq> offspring;
  offspring *children_;

  store *s_;
  
  unsigned num_relevant_;
  unsigned *relevant_;
    
  unsigned bytenum_;
  unsigned id_;
};

#endif
