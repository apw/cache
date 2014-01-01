#ifndef _LL_H
#define _LL_H

#include "rep.h"
#include <stdint.h>

class ll : public virtual rep {
 public:
  ll(const char *cur_time);
  ~ll(void);

  void do_add_byte(int id, unsigned bytenum, unsigned byteval);
  void prepare_to_query(void);
  unsigned do_query(uint8_t *bv, unsigned len);

 private:
  /* Data structure representing a tunnel.  */
  typedef struct tunnel {
    uint32_t c;                 /* Number of causal bytes.  */
    uint32_t *ci;               /* Array of causal byte indexes.  */
    uint8_t *cv;                /* Array of causal byte values.  */

    //uint32_t m;                 /* Number of modified bytes.  */
    //uint32_t *mi;               /* Array of modified byte indexes.  */
    //uint8_t *mv;                /* Array of modified byte values.  */

    uint64_t k;                 /* Length of the tunnel.  */

    struct tunnel *next;
  } tunnel_t;

  tunnel_t *head, *tail, *t;
  uint32_t j;

  int congruent(uint8_t *x, tunnel_t const *t);
};

#endif
