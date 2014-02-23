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

 protected:
  unsigned get_max_bytenum(uset_uint *done, uset_uint *u);
  void populate_subtrie(d_trie *d, uset_uint *done, uset_uint *u);
  
  vector<d_trie *> forest;
  
 private:
  typedef in_order super;
};

#endif
