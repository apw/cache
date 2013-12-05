#include "oracle.h"
#include <math.h>
#include <stdio.h>
#include <openssl/sha.h>

/* Print binary decision tree to stdout in the GraphViz representation
 * specialized to the fdp command.
 */
void fdp(tunnel * cache, int prune)
{
    uint8_t hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX context;
    tunnel *t;
    int i;

    /* Print header.  */
    printf("strict graph G {\n");
    printf("\tnode[ shape=point, width=0.3, style=filled, fillcolor=palegreen, "
           " fontname=\"helvetica\", fontsize=8];\n");

    /* Walk the linked list of cache entries.  */
    for (t = cache; t; t = t->next) {
        for (i = 0; i < t->l; i++) {
            /* If so configured, prune top of tree.  */
            if (i < prune)
                continue;

            /* Cryptographic hash of path from root to this vertex.  */
            SHA256_Init(&context);
            SHA256_Update(&context, t->code, i * sizeof(double));
            SHA256_Final(hash, &context);

            /* Print this vertex and begin printing edge to next vertex.  */
            printf("\t\"%02x%02x%02x%02x\" -- ",
                   hash[0], hash[1], hash[2], hash[3]);

            if (i < t->l - 1) {
                /* Cryptographic hash of path from root to next vertex.  */
                SHA256_Init(&context);
                SHA256_Update(&context, t->code, (i + 1) * sizeof(double));
                SHA256_Final(hash, &context);

                /* End printing an edge to next vertex.  */
                printf("\"%02x%02x%02x%02x\" \n",
                       hash[0], hash[1], hash[2], hash[3]);
            } else {
                /* End printing an edge to leaf vertex.  */
                printf("\"%p\" \n", t);

                /* Print next vertex.  */
                printf("\t\"%p\" "
                       "[shape=box3d, "
                       "label=<c<SUP>(%d)</SUP>>, "
                       "style=filled, " "fillcolor=red];\n", t, t->rank);
            }
        }
    }

    printf("}\n");
}
