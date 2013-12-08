#include "oracle.h"
#include <math.h>

/* Place cache entry t in binary decision tree v.  */
int place(vertex * v, tunnel * t, int d)
{
    int j, c = 0;

    /* If all ambiguity resolved, place cache entry and return.  */
    if (d >= t->l) {
        v->t = t;
        return 1;
    }

    /* Get d-th part of symbol code.  */
    j = t->code[d];

    /* If we are on the wrong branch of the decision tree, bail out.  */
    if (abs(j) != v->i)
        return 0;

    if (j < 0) {
        /* Allocate new node if necessary.  */
        if (v->left == 0) {
            v->left = calloc(1, sizeof(vertex));
            v->left->i = abs(t->code[d + 1]);
            c += 1;
        }
        /* Recurse on left child.  */
        c += place(v->left, t, d + 1);
    } else {
        /* Allocate new node if necessary.  */
        if (v->right == 0) {
            v->right = calloc(1, sizeof(vertex));
            v->right->i = abs(t->code[d + 1]);
            c += 1;
        }
        /* Recurse on right child.  */
        c += place(v->right, t, d + 1);
    }

    /* Return success code.  */
    return c;
}

/* Print binary decision tree.  */
void dump(vertex * v)
{
    if (v == 0)
        return;

    /* Print test.  */
    printf("%+d\n", v->i);

    /* Recurse on out-edges.  */
    dump(v->left);
    dump(v->right);
}

/* Place each entry in cache in binary decision tree root.  */
int tree(vertex * root, tunnel * cache)
{
    int c = 0;
    tunnel *t;

    /* Walk linked list of cache entries and place each entry in tree.  */
    for (t = cache; t; t = t->next)
        c += place(root, t, 0);

    /* Return number of nodes placed in tree.  */
    return c;
}
