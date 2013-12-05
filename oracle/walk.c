#include "oracle.h"
#include <math.h>

/* Walk binary decision tree v searching for match against
 * state vector x.
 */
tunnel *walk(uint8_t const *x, vertex * v)
{
    int b;

    /* If we have found a match, return the match.  */
    if (v->t)
        return v->t;

    /* Get bit from state vector tested by this vertex.  */
    b = bit(x, v->i);

    /* Recurse on appropriate child.  */
    if (b == +1 && v->right)
        return walk(x, v->right);
    else if (b == -1 && v->left)
        return walk(x, v->left);

    /* Return error if no match found.  */
    return 0;
}
