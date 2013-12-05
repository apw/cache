#include "oracle.h"
#include <stdlib.h>
#include <sys/param.h>

int constraint(tunnel * s, tunnel * t)
{
    int i;

    if (s == 0)
        return 1;

    for (i = 0; i < MAX(s->l, t->l); i++) {
        if (s->code[i] != t->code[i]) {
            if (i > 0 && (abs(s->code[i]) == abs(t->code[i]))) {
                return 1;
            }
            return 0;
        }
    }

    return 1;
}
