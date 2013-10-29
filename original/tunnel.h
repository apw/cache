#include <stdint.h>

/* Data structure representing a tunnel.  */
typedef struct tunnel {
    uint32_t c;                 /* Number of causal bytes.  */
    uint32_t *ci;               /* Array of causal byte indexes.  */
    uint8_t *cv;                /* Array of causal byte values.  */

    uint32_t m;                 /* Number of modified bytes.  */
    uint32_t *mi;               /* Array of modified byte indexes.  */
    uint8_t *mv;                /* Array of modified byte values.  */

    uint64_t k;                 /* Length of the tunnel.  */

    struct tunnel *next;
} tunnel_t;

/* Function signatures.  */
int congruent(uint8_t const *x, tunnel_t const *t);
tunnel_t *search(uint8_t const *x, tunnel_t *head);
