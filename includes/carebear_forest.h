#ifndef _CAREBEAR_FOREST_H
#define _CAREBEAR_FOREST_H

#include "rep.h"
#include "rset_uint.h"
#include "uset_uint.h"
#include "d_trie.h"

#include <stdint.h>
#include <iostream>
#include <tr1/unordered_map>
#include <tr1/unordered_set>
#include <vector>

using namespace std;

class carebear_forest : public in_order {
 public:
  carebear_forest(const char *cur_time);
  ~carebear_forest(void);

  void prepare_to_query(void);
  unsigned do_query(uint8_t *bv, unsigned len);

  /* TODO clearly worst design choice evar. where is my abstraction.. */
  // lock to protect num_finished_, result_, and cancel_version_
  pthread_mutex_t done_lock;
  
  // used with done_lock to signal the main thread that the worker query thread
  // finished
  pthread_cond_t done_cv;

  // variables protected by done_lock
  unsigned num_finished_;
  struct helper_args *result_;
  unsigned cancel_version_;

 protected:
  int get_max_bytenum(uset_uint *done, uset_uint *u, uset_uint *bytenums_left);
  void populate_subtrie(d_trie *d, uset_uint *done, uset_uint *u, uset_uint *bytenums_left);
  
  vector<d_trie *> forest_;

  unsigned max_relevant_bytenum_;
    
 private:
  typedef in_order super;
};

// argument struct for the worker query thread
struct helper_args {
  // reference to rep
  carebear_forest *cur;
  
  // trie to query
  d_trie *d;
  
  // byte vector to query
  uint8_t *bv;
  
  // number of steps taken
  unsigned steps;
  
  // if cancel_version is greater than cur->cancel_version_ result should not be
  // written back to cur->result_
  unsigned cancel_version;
  
  // stores the matching vector ID, or INVALID_ID if none found
  unsigned res;
};

void *do_query_helper(void *args);

#endif
