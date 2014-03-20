#ifndef _REP_H
#define _REP_H

#include <iostream>
#include <fstream>

#include <stdint.h>
#include <stddef.h>

using namespace std;

#define INVALID_ID ((unsigned) -1)
#define INVALID_BYTENUM ((unsigned) -1)

#define BUFLEN 256

class rep {
 public:
  rep(const char *cur_time, const char *outfile_basename);
  ~rep(void);

  void begin_sbv(int id);
  void add_byte(int id, unsigned bytenum, unsigned byteval);
  virtual void do_add_byte(int id, unsigned bytenum, unsigned byteval) = 0;
  void end_sbv(int id);

  unsigned query(uint8_t *bv, unsigned len);
  virtual void prepare_to_query(void) = 0;
  virtual unsigned do_query(uint8_t *bv, unsigned len) = 0;

  virtual void viz(void);

  unsigned get_num_hits(void);
  unsigned get_num_misses(void);

  char *get_outfile_name(void);
  const char *get_outfile_basename(void);

 protected:
  int current_id_;
  int num_steps_;
  const char *outfile_basename_;

 private:
  ofstream outfile_;
  unsigned num_hits_;
  unsigned num_misses_;
  
  char *outfile_name_;
};

#endif
