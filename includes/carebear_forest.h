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

  // clearly worst design choice evar. 
  pthread_mutex_t done_lock;
  pthread_cond_t done_cv;
  unsigned num_finished_;
  struct helper_args *result_;

 protected:
  int get_max_bytenum(uset_uint *done, uset_uint *u, uset_uint *bytenums_left);
  void populate_subtrie(d_trie *d, uset_uint *done, uset_uint *u, uset_uint *bytenums_left);
  
  vector<d_trie *> forest_;

  unsigned max_relevant_bytenum_;
    
 private:
  typedef in_order super;
};


struct helper_args {
  carebear_forest *cur;
  d_trie *d;
  uint8_t *bv;
  unsigned steps;
  unsigned res;
};

void *do_query_helper(void *args);

#endif
