#define _FILE_OFFSET_BITS 64
#define __USE_LARGEFILE64

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <limits.h>

#include <cstring>
#include <vector>
#include <tr1/memory>

#include "../includes/rep.h"
#include "../includes/in_order.h"
#include "../includes/simple_cb.h"
#include "../includes/trie_cb.h"
#include "../includes/greedy_trie.h"
#include "../includes/carebear_dual_trie.h"
#include "../includes/carebear_forest.h"
#include "../includes/greedy_forest.h"
#include "../includes/batch_forest.h"
#include "../includes/lazy_exp.h"
#include "../includes/ll.h"
#include "../includes/cycle_timing.h"
#include "../includes/common.h"
#include "../includes/verify_results.h"

using namespace std;

#define NUM_ARGS 2
#define MAX_FNAME_LEN 256
#define PRINT_INTERVAL 10000

typedef enum {LL_IMP, IN_ORDER_IMP, SIMPLE_CB_IMP,
	      TRIE_CB_IMP, CAREBEAR_DUAL_TRIE_IMP,
	      CAREBEAR_FOREST_IMP, LAZY_EXP_IMP,
	      GREEDY_TRIE_IMP, GREEDY_FOREST_IMP,
	      BATCH_FOREST_IMP} imp_t;

// order in which implementations will be run
int imps[] = {GREEDY_TRIE_IMP, GREEDY_FOREST_IMP, CAREBEAR_FOREST_IMP, CAREBEAR_DUAL_TRIE_IMP, LL_IMP, SIMPLE_CB_IMP, TRIE_CB_IMP,
	      LAZY_EXP_IMP};
//int imps[] = {BATCH_FOREST_IMP};

#define TIMEBUF_SZ 80
char *cur_time;

// passed in at runtime
char *cache_file_name;
char *query_file_name;

FILE *cache_file = NULL;
FILE *query_file = NULL;

typedef tr1::shared_ptr<rep> rep_ptr;

/*
 * We want to only have one representation active in memory
 * at any particular time since each representation is likely to take up a lot
 * of memory. Initializes the "imp_num"th representation
 * and returns a rep_ptr to it.
 */
static rep_ptr initialize_rep(int imp_num) {
  rep_ptr r;
  switch(imp_num) {
  case IN_ORDER_IMP: {
    in_order *i = new in_order(cur_time);
    r.reset(i);
    break;
  }
  case LL_IMP: {
    ll *i = new ll(cur_time);
    r.reset(i);
    break;
  }
  case SIMPLE_CB_IMP: {
    pterodactyl();
    simple_cb *i = new simple_cb(cur_time);
    r.reset(i);
    break;
  }
  case TRIE_CB_IMP: {
    trie_cb *i = new trie_cb(cur_time);
    r.reset(i);
    break;
  }
  case CAREBEAR_DUAL_TRIE_IMP: {
    bears();
    carebear_dual_trie *i = new carebear_dual_trie(cur_time);
    r.reset(i);    
    break;
  }
  case GREEDY_TRIE_IMP: {
    greedy_trie *i = new greedy_trie(cur_time);
    r.reset(i);
    break;
  }
  case CAREBEAR_FOREST_IMP: {
    forest();
    carebear_forest *i = new carebear_forest(cur_time);
    r.reset(i);    
    break;
  }
  case GREEDY_FOREST_IMP: {
    greedy_forest *i = new greedy_forest(cur_time);
    r.reset(i);    
    break;
  }
  case BATCH_FOREST_IMP: {
    batch_forest *i = new batch_forest(cur_time);
    r.reset(i);    
    break;
  }
  case LAZY_EXP_IMP: {
    snorlax();
    lazy_exp *i = new lazy_exp(cur_time);
    r.reset(i);
    break;
  }
  default: {
    assert(0);
    break;
  }
  }

  rep_ptr ret(r);
  return ret;
}

/*
 * cleans up a rep_ptr
 */
void cleanup(rep_ptr r) {
  return;
}

/*
 * Loads the cache-entries from disk into the rp representation
 */
void load(rep_ptr rp) {
  unsigned bytenum;
  uint8_t byteval;
  int id = -1;
  
  // TODO change all add_byte calls to take in uint8_t instead of unsigned
  for (unsigned j = UINT_MAX; fscanf(cache_file, "%u %hhu", &bytenum, &byteval) != EOF; j = bytenum) {
    if (bytenum < j) {
      if (j != UINT_MAX) {
	rp->end_sbv(id);
      }

      id++;      

      rp->begin_sbv(id);
    }

    rp->add_byte(id, bytenum, byteval);
  }

  assert(errno == 0);
}

/*
 * For each query in the query file, reads each bytenum-byteval pair
 * and queries the cache about whether it was present in the cache or not
 */
void run(rep_ptr rpt) {
  unsigned bytenum;
  uint8_t byteval;
  unsigned bv_len = ((unsigned) -1);
  uint8_t *bv = NULL;
  unsigned query_count = 0;

  for (unsigned j = UINT_MAX; fscanf(query_file, "%u %hhu", &bytenum, &byteval) != EOF; j = bytenum) {
    if (bytenum < j && j != UINT_MAX) {
      bv_len = j + 1;
      assert(bv_len > 0);
      
      bv = (uint8_t *) calloc(bv_len, sizeof(uint8_t));	
      assert(bv != NULL);
      
      break;
    }
  }

  rewind(query_file);
  assert(errno == 0);

  for (unsigned j = UINT_MAX; fscanf(query_file, "%u %hhu", &bytenum, &byteval) != EOF; j = bytenum) {
    if (bytenum < j) {
      if (query_count % PRINT_INTERVAL == 0) {
	printf("\r[%u]", query_count);
      }

      fflush(stdout);
      query_count++;
      
      rpt->query(bv, bv_len);
    }

    bv[bytenum] = byteval;
  }

  assert(bv != NULL);
  free(bv);
  assert(errno == 0);

  printf("\n");
}

int main(int argc, char **argv) {
  if (argc != NUM_ARGS + 1) {
    printf("Usage: %s [cache_file] [query_file]\n", argv[0]);
    return 1;
  }
  
  cache_file_name = argv[1];
  query_file_name = argv[2];

  cur_time = (char *) malloc(sizeof(char) * TIMEBUF_SZ);
  get_str_time(cur_time, TIMEBUF_SZ);
  
  // TODO do we need to call this right before we time?
  int64_t func_call_overhead = warmup_time();
  (void) func_call_overhead;

  // array to remember outfile names
  int num_reps = sizeof(imps)/sizeof(*imps);
  char *outfile_names[num_reps];
  for (int i = 0; i < num_reps; i++) {
    outfile_names[i] = (char *) calloc(MAX_FNAME_LEN, sizeof(char));
  }

  unsigned first_hits = 0, first_misses = 0;
  for(int i = 0; i < num_reps; i++) {
    printf("opening cache file\n");
    cache_file = fopen(cache_file_name, "r");
    if (!cache_file) {
      printf("Could not open cache_file \"%s\".\n", cache_file_name);
      return 3;
    }
    
    printf("opening query file\n");
    query_file = fopen(query_file_name, "r");
    if (!query_file) {
      fclose(cache_file);
      printf("Could not open query_file \"%s\": %s\n", query_file_name, strerror(errno));
      return 4;
    }

    printf("initializing rep %d\n", i);
    rep_ptr r = initialize_rep(imps[i]);
    assert(r);

    printf("loading rep %d: %s\n", i, r->get_outfile_basename());
    load(r);

    printf("preparing to query rep %d\n", i);
    fflush(stdout);
    r->prepare_to_query();
    
    printf("running rep %d\n", i);
    fflush(stdout);
    run(r);

    r->viz();
    
    strcpy(outfile_names[i], r->get_outfile_name());

    if (i == 0) {
      first_hits = r->get_num_hits();
      first_misses = r->get_num_misses();
    } else {
      assert(first_hits == r->get_num_hits());
      assert(first_misses == r->get_num_misses());
    }

    printf("cleaning up rep %d\n", i);
    cleanup(r);
    
    printf("closing cache file %d\n", i);
    fclose(cache_file);

    printf("closing query file %d\n", i);
    fclose(query_file);
    
  }

  verify_results(num_reps, outfile_names);
  
  for (int i = 0; i < num_reps; i++) {
    free(outfile_names[i]);
  }

  free(cur_time);
  
  return 0;
}
