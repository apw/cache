#ifndef _SIMPLE_CB_H
#define _SIMPLE_CB_H

#include "rep.h"
#include "in_order.h"
#include <stdint.h>
#include <iostream>
#include <tr1/unordered_map>
#include <tr1/unordered_set>
#include <vector>

using namespace std;

class simple_cb : public in_order {
 public:
  simple_cb(const char *cur_time);

  void prepare_to_query(void);

 protected:
  class bytenum_prio {
  public:
    unsigned bytenum;
    unsigned prio;
  };

  struct prio_compare {
    bool operator() (bytenum_prio a, bytenum_prio b) {
      // STORY 1
      if (a.prio != b.prio) {
	return a.prio > b.prio;
      }

      return a.bytenum < b.bytenum;
    }
  } prio_compare;

 private:
  typedef in_order super;
};

#endif
