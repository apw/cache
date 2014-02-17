#ifndef _CAREBEAR_DUAL_TRIE_H
#define _CAREBEAR_DUAL_TRIE_H

#include "rep.h"
#include "trie_cb.h"
#include <stdint.h>
#include <iostream>
#include <tr1/unordered_map>
#include <tr1/unordered_set>
#include <vector>

using namespace std;

class carebear_dual_trie : public trie_cb {
 public:
  carebear_dual_trie(const char *cur_time);

 protected:

  void order_bytenums(void);
  
  class carebear_prio {
  public:
    unsigned bytenum;
    unsigned num_care;
    unsigned variability;
  };

  struct carebear_compare {
    bool operator() (carebear_prio a, carebear_prio b) {
      if (a.num_care != b.num_care) {
	return a.num_care < b.num_care;
      } else if (a.variability != b.variability) {
	return a.variability > b.variability;
      }
      
      return a.bytenum < b.bytenum;
    }
  } carebear_compare;

 private:
  typedef trie_cb super;
};

#endif
