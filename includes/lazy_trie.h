#ifndef _LAZY_TRIE_H
#define _LAZY_TRIE_H

#include "rep.h"
#include <tr1/unordered_map>
#include <vector>
#include <stdint.h>

using namespace std;

class lazy_trie {
 public:
  lazy_trie(unsigned bytenum, unsigned id,
	    unsigned cur_index, unsigned num_relevant, unsigned *relevant);
  ~lazy_trie(void);
  
  lazy_trie *decide(uint8_t byteval);
  bool is_leaf(void);
  
  unsigned get_bytenum(void);
  unsigned get_id(void);
  
  void print(void);

  typedef struct {
    unsigned bytenum;
    uint8_t byteval;
  } bytenumval;

  typedef vector<bytenumval> v_entries;
  
  typedef struct {
    v_entries ve;
    unsigned id;
  } c_entry;
  
  void add_vect(c_entry ce);
  
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

  typedef vector<c_entry> lazy_store;
  lazy_store *s_;
  
  unsigned num_relevant_;
  unsigned cur_index_;
  unsigned *relevant_; // TODO who frees this?
    
  unsigned bytenum_;
  unsigned id_;
};

#endif
