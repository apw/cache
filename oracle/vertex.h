#include <stdbool.h>

typedef struct vertex {
    unsigned int i;             /* The bit tested by this vertex.  */

    tunnel *t;                  /* Payload if this vertex is a leaf.  */

    struct vertex *left;        /* Out edge.  */
    struct vertex *right;       /* Out edge.  */
} vertex;
