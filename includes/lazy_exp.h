#ifndef _LAZY_EXP_H
#define _LAZY_EXP_H

#include "rep.h"
#include "simple_cb.h"
#include "lazy_trie.h"

#include <vector>

class lazy_exp : public simple_cb {
 public:
  lazy_exp(const char *cur_time);
  ~lazy_exp(void);

  void do_add_byte(int id, unsigned bytenum, unsigned byteval);
  void prepare_to_query(void);
  unsigned do_query(uint8_t *bv, unsigned len);
  void begin_sbv(int id);
  void end_sbv(int id);

 protected:
  lazy_trie *z_;
  
 private:
  typedef simple_cb super;
};

#endif
