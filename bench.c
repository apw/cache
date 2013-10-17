#include <limits.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#include "rdtsc.h"
#include "tunnel.h"

int main(int argc, char *argv[])
{
    tunnel_t *t, *head = 0, *tail = 0;
    unsigned long long dt;
    int64_t lo, tic, toc;
    FILE *cache, *query;
    uint8_t cv, *x = 0;
    uint32_t ci, j;
    int i;

    /* Check for arguments.  */
    if (argc != 3) {
        printf("Usage: %s CACHE QUERIES\n", argv[0]);
        goto failure;
    }

    /* Open the tunnel cache file.  */
    cache = fopen(argv[1], "r");

    /* Bail out if error opening cache file.  */
    if (cache == 0) {
        perror("fopen");
        goto failure;
    }

    /* Open the query file.  */
    query = fopen(argv[2], "r");

    /* Bail out if error opening query file.  */
    if (query == 0) {
        perror("fopen");
        goto failure;
    }

    /* Read in the cache file.  */
    for (j = UINT_MAX; fscanf(cache, "%u %hhu", &ci, &cv) != EOF; j = ci) {
        /* If this is a new cache entry set up a new tunnel.  */
        if (ci < j) {
            /* Allocate tunnel data structure.  */
            t = calloc(1, sizeof(tunnel_t));

            /* Add new tunnel to cache.  */
            if (head == 0)
                head = t;
            if (tail != 0)
                tail->next = t;
            tail = t;
        }

        /* Increment number of dependencies for this tunnel.  */
        t->c++;

        /* Store the index.  */
        t->ci = realloc(t->ci, t->c * sizeof(uint32_t));
        t->ci[t->c - 1] = ci;

        /* Store the value.  */
        t->cv = realloc(t->cv, t->c * sizeof(uint8_t));
        t->cv[t->c - 1] = cv;
    }

    /* Dump the cache if so configured.  */
    if (0) {
        for (t = head; t != 0; t = t->next) {
            for (i = 0; i < t->c; i++) {
                printf("%d %d ", t->ci[i], t->cv[i]);
            }
            printf("\n");
        }

        goto success;
    }

    /* Calibrate the cycle counter.  */
    lo = rdtsc(1);

    /* Read in the query file and time each query against the cache.  */
    for (j = 0; fscanf(query, "%u %hhu", &ci, &cv) != EOF; j = ci) {
        /* If this is a new state vector, query the cache on the old one.  */
        if (ci < j) {
            /* Time cache search for old state vector.  */
            tic = rdtsc(0);
            t = search(x, head);
            toc = rdtsc(0);

            /* Calculate the calibrated cycle count.  */
            dt = toc - tic - lo;

            /* Print hit indicator and calibrated cycle count.  */
            if (printf("%d %lld\n", t != 0, dt) < 0)
                goto failure;
        }

        /* Extend state vector with new entry.  */
        x = realloc(x, (ci + 1) * sizeof(uint32_t));
        x[ci] = cv;
    }

    /* Exit with success code.  */
  success:
    return 0;

    /* Exit with error code.  */
  failure:
    return 1;
}
