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
  //lazy_trie(unsigned bytenum, unsigned id);
  lazy_trie(unsigned bytenum, unsigned id,
	    unsigned cur_index, unsigned num_relevant, unsigned *relevant);
  ~lazy_trie(void);
    
  lazy_trie *decide(uint8_t byteval);
  void add_vect(vect bv, unsigned id);
  bool is_leaf(void);
  
  unsigned get_bytenum(void);
  unsigned get_id(void);
  
  void print(void);
  
 protected:
  bool is_lazy(void);
  
  void burst(void);
  
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
  
  unsigned num_relevant_; // TODO set this
  unsigned cur_index_; // TODO set this
  unsigned *relevant_; // TODO set this
    
  unsigned bytenum_;
  unsigned id_;
};

#endif
