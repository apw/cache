#ifndef _BATCH_FOREST_H
#define _BATCH_FOREST_H

#include "rep.h"
#include "carebear_forest.h"
#include "uset_uint.h"
#include "d_trie.h"

#include <stdint.h>
#include <iostream>
#include <tr1/unordered_map>
#include <tr1/unordered_set>
#include <vector>

using namespace std;

class batch_forest : public carebear_forest {
 public:
  batch_forest(const char *cur_time);
  
  void prepare_to_query(void);
 protected:
  typedef vector<vector<c_entry>> batches;
  
  void construct_forest(batches *b);
  int get_max_bytenum(uset_uint *done, uset_uint *u,
		      uset_uint *bytenums_left, uset_uint *batch_vectors);

  
 private:
  typedef carebear_forest super;

};

#endif
