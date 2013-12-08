#include "oracle.h"
#include <math.h>

/* Return bipolar representation of the j-th bit of state vector x.  */
int bit(uint8_t const *x, int j)
{
    int b;

    b = pow(-1, (x[j / 8] >> (j % 8)) & 0x1);

    return b;
}
