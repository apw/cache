#ifndef _REP_H
#define _REP_H

#include <iostream>
#include <fstream>

#include <stdint.h>
#include <stddef.h>
#include <boost/unordered_map.hpp>

#pragma once

using namespace std;

class rep {
 public:
  rep(const char *cur_time, const char *outfile_basename);
  ~rep(void);

  void begin_sbv(int id);
  void add_byte(int id, unsigned bytenum, unsigned byteval);
  virtual void do_add_byte(int id, unsigned bytenum, unsigned byteval) = 0;
  void end_sbv(int id);

  bool query(uint8_t *bv, unsigned len);
  virtual void prepare_to_query(void) = 0;
  virtual int do_query(uint8_t *bv, unsigned len) = 0;

 protected:
  int current_id_;
  int num_steps_;

 private:
  ofstream outfile_;
  unsigned num_hits_;
  unsigned num_misses_;
};



#endif
