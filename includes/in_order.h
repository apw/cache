/* Interface for in_order, which is the root implementation of TeraVectyl.
 * Other implementations of TeraVectyl (such as simple_cb) extend this root
 * implementation.
 */

#ifndef _IN_ORDER_H
#define _IN_ORDER_H

#include "rep.h"
#include "common_types.h"
#include "rset_uint.h"

#include <stdint.h>
#include <iostream>
#include <tr1/unordered_map>
#include <tr1/unordered_set>
#include <vector>

using namespace std;

#define SHORT_CIRCUIT_THRESHOLD 1

class in_order : public virtual rep {
 public:
  in_order(const char *cur_time);
  ~in_order(void);

  void do_add_byte(int id, unsigned bytenum, unsigned byteval);
  void prepare_to_query(void);
  unsigned do_query(uint8_t *bv, unsigned len);
  void begin_sbv(int id);
  void end_sbv(int id);

 protected:
  rset_uint *candidates_;

  cache c_;

  unsigned num_relevant_;
  unsigned *relevant_;

  store s_;

 private:
  typedef rep super;
};

#endif
