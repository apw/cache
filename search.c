#include "tunnel.h"

/* Walk the linked list of cached tunnel entries.
 * Return the first tunnel whose begin state vector is 
 * equivalent to x up to symmetries.
 * Return 0 if no match found.
 */
tunnel_t *search(uint8_t const *x, tunnel_t *head)
{
    tunnel_t *t = 0;

    for (t = head; t; t = t->next)
        if (congruent(x, t))
            return t;

    return t;
}
