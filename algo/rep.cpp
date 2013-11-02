#include "../includes/rep.h"
#include "../includes/cycle_timing.h"

#include <iostream>
#include <fstream>

#include <assert.h>
#include <stdint.h>

using namespace std;

rep::rep(const char *outfile_name, const char *cur_time) {
  current_id_= 0;
  num_hits_ = 0;
  num_misses_ = 0;
  
  char buf[256];
  snprintf(buf, sizeof(buf), "output/%s_%s.raw", outfile_name, cur_time);

  outfile_.open(buf);
  assert(outfile_.is_open());
}

rep::~rep() {
  assert(outfile_.is_open());
  outfile_.close();
}

void rep::begin_sbv(int id) {
  assert(id != 0);
  assert(current_id_ + 1 == id);
  current_id_ = id;
}

void rep::add_byte(int id, unsigned bytenum, unsigned byteval) {
  assert(id != 0);
  assert(id == current_id_);
  do_add_byte(id, bytenum, byteval);
}

void rep::end_sbv(int id) {
  assert(id != 0);
  assert(id == current_id_);
}

bool rep::query(uint8_t *bv, unsigned len) {
  int hit;
  int64_t time = time_magic(this, &rep::do_query, bv, len, &hit);

  // TODO verify times are logged correctly in the file
  if (hit) {
    num_hits_++;
    outfile_ << "H " << time << endl;
  } else {
    num_misses_++;
    outfile_ << "M " << time << endl;
  }

  assert(!outfile_.fail());
  return hit;
}
