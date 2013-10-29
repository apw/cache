#include "tunnel.h"

/* Walk the linked list of cached tunnel entries.
 * Return the first tunnel whose begin state vector is 
 * equivalent to state vector x up to symmetries.
 * Return 0 if no match found.
 */
tunnel_t *search(uint8_t const *x, tunnel_t *head)
{
    tunnel_t *t;

    for (t = head; t; t = t->next)
        if (congruent(x, t))
            return t;

    return 0;
}
