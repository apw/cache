#ifndef _TRIE_CB_H
#define _TRIE_CB_H

#include "rep.h"
#include "simple_cb.h"
#include "d_trie.h"
#include "uset_uint.h"

#include <vector>

class trie_cb : public simple_cb {
 public:
  trie_cb(const char *cur_time);
  ~trie_cb(void);

  void do_add_byte(int id, unsigned bytenum, unsigned byteval);
  void prepare_to_query(void);
  unsigned do_query(uint8_t *bv, unsigned len);
  void begin_sbv(int id);
  void end_sbv(int id);

 protected:
  d_trie *d_;

 private:
  typedef simple_cb super;
  
  void populate_trie(d_trie *, uset_uint *, unsigned);
  unsigned do_query_helper(d_trie *, uint8_t *, unsigned);
};

#endif
