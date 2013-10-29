#include "tunnel.h"

/* Return 1 if x == y mod g, where the tunnel t represents the
 * symmetries g of a previously-solved trajectory from y to z.
 * Return 0 otherwise.
 */
int congruent(uint8_t const *x, tunnel_t const *t)
{
    uint32_t i, j;
    uint8_t byte;

    /* Walk through the c causal coordinates of tunnel t.  */
    for (i = 0; i < t->c; i++) {
        /* Get the tunnel's i-th causal coordinate.  */
        j = t->ci[i];

        /* Get the value of the tunnel's i-th causal coordinate.  */
        byte = t->cv[i];

        /* Bail out early when any byte does not match.  */
        if (x[j] != byte)
            return 0;
    }

    return 1;
}
