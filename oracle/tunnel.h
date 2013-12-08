#include <stdint.h>
#include <stdlib.h>
#include <gsl/gsl_permutation.h>

/* Data structure representing a tunnel.  */
typedef struct tunnel {
    uint32_t c;                 /* Number of causal bytes.  */
    uint32_t *ci;               /* Array of causal byte indexes.  */
    uint8_t *cv;                /* Array of causal byte values.  */

    uint32_t m;                 /* Number of modified bytes.  */
    uint32_t *mi;               /* Array of modified byte indexes.  */
    uint8_t *mv;                /* Array of modified byte values.  */

    uint64_t k;                 /* Length of the tunnel.  */

    int rank;                   /* Used for assigning to vertex in tree.  */

    int l;
    double *code;
    gsl_permutation *pi;

    struct tunnel *prev;        /* Previous node in linked list.  */
    struct tunnel *next;        /* Next node in linked list.  */
} tunnel;

/* Function signatures.  */
int congruent(uint8_t const *x, tunnel const *t);
tunnel *search(uint8_t const *x, tunnel * head);
