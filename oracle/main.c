#define _GNU_SOURCE
#include "oracle.h"
#include "rdtsc.h"
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int c, i, d, flags, n, m = 0, C = 0, prune = 0;
    char *line = 0, *cache = 0, *query = 0;
    tunnel *t, *head = 0, *tail = 0;
    FILE *file[2] = { 0 };
    int64_t lo, tic, toc;
    uint8_t *x, **X = 0;
    vertex *root = 0;
    uint32_t ci, j;
    size_t cap = 0;
    long long dt;
    gsl_rng *r;
    uint8_t cv;

    /* Loop through argument array updating the options struct.  */
    for (flags = 0; (c = getopt(argc, argv, "h1234567c:q:rp:")) != -1;) {
        switch (c) {
        case 'h':              /* Print help statement then exit.  */
            flags |= HELP;
            break;
        case '1':              /* Dump cache then exit.  */
            flags |= CACHE;
            break;
        case '2':              /* Dump raw code then exit.  */
            flags |= RAW;
            break;
        case '3':              /* Dump symbol code then exit.  */
            flags |= SYMBOL;
            break;
        case '4':
            flags |= DOT;
            break;
        case '5':
            flags |= FDP;
            break;
        case '6':
            flags |= QUERY;
            break;
        case '7':
            flags |= BITS;
            break;
        case 'c':              /* Read cache from CACHE.  */
            cache = optarg;
            break;
        case 'q':              /* Read queries from QUERY.  */
            query = optarg;
            break;
        case 'r':
            flags |= LIST;
            break;
        case 'p':
            prune = strtol(optarg, 0, 10);
            break;
        default:               /* Unrecognized option.  */
            flags |= ERROR;
            goto failure;
        }
    }

    /* Bail out if there are unexpected arguments.  */
    if (optind < argc) {
        fprintf(stderr, "%s: trailing garbage: %s\n", argv[0], argv[optind]);
        goto failure;
    }

    /* Print usage statement and exit if necessary.  */
    if (flags & HELP || argc < 2) {
        help(argc, argv);
        goto success;
    }

    /* Set up the random number generator.  */
    gsl_rng_env_setup();
    r = gsl_rng_alloc(gsl_rng_default);

    /* Read contents of cache file.  */
    if (cache) {
        /* Open cache file.  */
        file[0] = fopen(cache, "r");

        /* Bail out if error opening cache file.  */
        if (file[0] == 0) {
            perror("fopen: cache");
            goto failure;
        }

        /* Read next line of cache file.  */
        for (i = 0, C = 0; getline(&line, &cap, file[0]) != -1; i++, C++) {
            /* Allocate tunnel data structure.  */
            t = calloc(1, sizeof(tunnel));

            /* Identify this cache entry.  */
            t->rank = i;

            /* Append this cache entry to linked list.  */
            if (head == 0) {
                head = tail = t;
            } else {
                tail->next = t;
                t->prev = tail;
                tail = t;
            }

            /* Read index/value pairs from current line of cache file.  */
            while (sscanf(line, "%u %hhu%n", &ci, &cv, &d) == 2) {
                /* Increment number of dependencies for this tunnel.  */
                t->c++;

                /* Store the index.  */
                t->ci = realloc(t->ci, t->c * sizeof(uint32_t));
                t->ci[t->c - 1] = ci;

                /* Store the value.  */
                t->cv = realloc(t->cv, t->c * sizeof(uint8_t));
                t->cv[t->c - 1] = cv;

                /* Assign the identity code.  */
                for (j = 0; j < 8; j++) {
                    t->l++;
                    t->code = realloc(t->code, t->l * sizeof(double));
                    c = pow(-1, (cv >> j) & 0x1) * (8 * ci + j);
                    t->code[t->l - 1] = c;
                }

                /* Update line pointer.  */
                line += d;
            }

            /* Reset current line information.  */
            line = 0;
            cap = 0;
        }

        /* Dump the cache if so configured.  */
        if (flags & CACHE) {
            /* Walk through tunnels printing each.  */
            for (t = head; t != 0; t = t->next) {
                /* Print entries in same format as input.  */
                for (i = 0; i < t->c; i++)
                    printf("%d %d ", t->ci[i], t->cv[i]);
                printf("\n");
            }

            /* Exit with success code.  */
            goto success;
        }

        /* Print the identity code if so configured.  */
        if (flags & RAW) {
            /* Walk through tunnels printing each.  */
            for (t = head; t != 0; t = t->next) {
                /* Print portion of codeword for the current bit.  */
                for (i = 0; i < t->l; i++)
                    printf("%+g ", t->code[i]);
                printf("\n");
            }

            /* Exit with success code.  */
            goto success;
        }

        /* Solve for a prefix coding of the system.  */
        c = solve(head, r);

        /* Bail out if no prefix code found.  */
        if (c == 0) {
            fprintf(stderr, "%s: error: no prefix code found\n", argv[0]);
            goto failure;
        }

        /* Print the solved encoding if so configured.  */
        if (flags & SYMBOL) {
            /* Walk the linked list of cache entries.  */
            for (t = head; t != 0; t = t->next) {
                /* Print portion of code for each bit.  */
                for (i = 0; i < t->l; i++)
                    printf("%+g ", t->code[i]);
                printf("\n");
            }

            /* Exit with success code.  */
            goto success;
        }

        /* Initialize root of binary decision tree.  */
        root = calloc(1, sizeof(vertex));
        root->i = abs(head->code[0]);

        /* Build decision tree using the solved-for symbol code.  */
        c = tree(root, head);

        /* Bail out if error building decision tree.  */
        if (c < 1) {
            fprintf(stderr, "%s: error: decision tree failed\n", argv[0]);
            goto failure;
        }

        /* Emit graph appropriate for dot if so configured.  */
        if (flags & DOT) {
            dot(head, prune);

            /* Exit with success code.  */
            goto success;
        }

        /* Emit graph appropriate for fdp if so configured.  */
        if (flags & FDP) {
            fdp(head, prune);

            /* Exit with success code.  */
            goto success;
        }
    }

    /* Read contents of query file.  */
    if (query) {
        /* Open query file.  */
        file[1] = fopen(query, "r");

        /* Bail out if error opening query file.  */
        if (file[1] == 0) {
            perror("query file fopen");
            goto failure;
        }

        /* Read next line of query file.  */
        for (m = 0, n = 0; getline(&line, &cap, file[1]) > 0; m++) {
            /* Increase row dimension of query matrix.  */
            X = realloc(X, (m + 1) * sizeof(uint8_t *));

            /* Initialize new row vector.  */
            x = 0;

            /* Fill out new row vector.  */
            for (; sscanf(line, "%u %hhu%n", &ci, &cv, &d) == 2; line += d) {
                /* Increase column dimension of current row vector.  */
                x = realloc(x, (ci + 1) * sizeof(uint8_t));

                /* Append new scalar to current row vector.  */
                x[ci] = cv;

                /* Update maximum column dimension.  */
                if (ci + 1 > n)
                    n = ci + 1;
            }

            /* Append new row vector to query matrix.  */
            X[m] = x;

            /* Reset line information.  */
            line = 0;
            cap = 0;
        }

        /* Dump queries if so configured.  */
        if (flags & QUERY) {
            /* Walk through query matrix printing each row.  */
            for (i = 0; i < m; i++) {
                /* Print entries in same format as input.  */
                for (j = 0; j < n; j++)
                    printf("%d %d ", j, X[i][j]);
                printf("\n");
            }

            /* Exit with success code.  */
            goto success;
        }

        /* Dump queries as boolean vectors if so configured.  */
        if (flags & BITS) {
            /* Print each row of query matrix as a boolean vector.  */
            for (i = 0; i < m; i++) {
                /* Get current query vector.  */
                x = X[i];

                for (j = 0; j < 8 * n; j++)
                    printf("%+d ", bit(x, j));
                printf("\n");
            }

            /* Exit with success code.  */
            goto success;
        }
    }

    /* Bail out if either required file is not open.  */
    if (file[0] == 0) {
        fprintf(stderr, "%s: error: missing cache file\n", argv[0]);
        goto failure;
    } else if (file[1] == 0) {
        fprintf(stderr, "%s: error: missing query file\n", argv[0]);
        goto failure;
    }

    /* Print header.  */
    printf("%-8s %-8s %-8s %-8s %-8s\n", "hit", "cycles", "C", "m", "tree");
    fflush(stdout);

    /* Calibrate cycle counter.  */
    lo = rdtsc(1);

    /* Count number of cycles consumed by each query.  */
    for (i = 0; i < m; i++) {
        /* Get current query vector.  */
        x = X[i];

        /* Count number of cycles consumed by this query.  */
        if (flags & LIST) {
            /* Baseline ASC linked list.  */
            tic = rdtsc(0);
            t = search(x, head);
            toc = rdtsc(0);
        } else {
            /* Oracle decision tree.  */
            tic = rdtsc(0);
            t = walk(x, root);
            toc = rdtsc(0);

            /* Make sure the oracle is telling the truth.  */
            if (congruent(x, t) == 0) {
                fprintf(stderr, "panic: false positive\n");
                raise(SIGTRAP);
            }
        }

        /* Calculate the calibrated cycle count for query.  */
        dt = toc - tic - lo;

        /* Print hit indicator and calibrated cycle count.  */
        c = printf("%-8d %-8lld %-8d %-8d %-8d\n",
                   t != 0, dt, C, m, !(flags & LIST));

        /* Play nicely in shell pipelines.  */
        if (c < 0)
            goto failure;
        fflush(stdout);
    }

    /* Exit with success code.  */
  success:
    return 0;

    /* Exit with error code.  */
  failure:
    return 1;
}
