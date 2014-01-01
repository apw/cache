#include "../includes/ll.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

ll::ll(const char *cur_time) : rep(cur_time, "ll") {
  head = NULL;
  tail = NULL;
  j = UINT_MAX;
}

ll::~ll() {
  tunnel *cur = head;
  tunnel *old_cur;
  while (cur) {
    free(cur->ci);
    free(cur->cv);
    old_cur = cur;
    cur = cur->next;
    free(old_cur);
  }
}

void ll::do_add_byte(int id, unsigned bytenum, unsigned byteval) {
  /* If this is a new cache entry set up a new tunnel.  */
  if (bytenum < j) {
    /* Allocate tunnel data structure.  */
    t = (tunnel_t *) calloc(1, sizeof(tunnel_t));
    
    /* Add new tunnel to cache.  */
    if (head == 0)
      head = t;
    if (tail != 0)
      tail->next = t;
    tail = t;
  }
  
  /* Increment number of dependenbytenumes for this tunnel.  */
  t->c++;
  
  /* Store the index.  */
  t->ci = (uint32_t *) realloc(t->ci, t->c * sizeof(uint32_t));
  t->ci[t->c - 1] = bytenum;
  
  /* Store the value.  */
  t->cv = (uint8_t *) realloc(t->cv, t->c * sizeof(uint8_t));
  t->cv[t->c - 1] = byteval;

  j = bytenum;
}

void ll::prepare_to_query() {
  
}

unsigned ll::do_query(uint8_t *x, unsigned len) {
  tunnel_t *tun;

  for (tun = head; tun; tun = tun->next) {
    if (congruent(x, tun)) {
      return 1;
    }
  }
  
  return INVALID_ID;
}

int ll::congruent(uint8_t *x, tunnel_t const *t) {
  uint32_t i, k;
  uint8_t byte;
  
  /* Walk through the c causal coordinates of tunnel t.  */
  for (i = 0; i < t->c; i++) {
    num_steps_++;

    /* Get the tunnel's i-th causal coordinate.  */
    k = t->ci[i];
    
    /* Get the value of the tunnel's i-th causal coordinate.  */
    byte = t->cv[i];
    
    /* Bail out early when any byte does not match.  */
    if (x[k] != byte) {
      return 0;
    }
  }

  // TODO return ID
  return 1;
}
