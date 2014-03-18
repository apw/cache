#ifndef _GREEDY_FOREST_H
#define _GREEDY_FOREST_H

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

class greedy_forest : public carebear_forest {
 public:
  greedy_forest(const char *cur_time);
  
 protected:
  int get_max_bytenum(uset_uint *done, uset_uint *u, uset_uint *bytenums_left);
  
 private:
  typedef carebear_forest super;
};

#endif
