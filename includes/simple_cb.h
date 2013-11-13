#ifndef _SIMPLE_CB_H
#define _SIMPLE_CB_H

#include "rep.h"
#include "in_order.h"
#include <stdint.h>
#include <iostream>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include <vector>

using namespace std;
using namespace boost;

#pragma once

class simple_cb : public in_order {
 public:
  simple_cb(const char *cur_time);

  void prepare_to_query(void);

 private:
  typedef in_order super;

  class bytenum_prio {
  public:
    unsigned bytenum;
    unsigned prio;
  };

  struct {
    bool operator() (bytenum_prio a, bytenum_prio b) {
      if (a.prio != b.prio) {
	return a.prio < b.prio;
      }
      return a.bytenum < b.bytenum;

      //return a.prio < b.prio;
    }
  } prio_compare;
};

#endif
