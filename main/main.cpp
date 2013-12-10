#define _FILE_OFFSET_BITS 64
#define __USE_LARGEFILE64

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <string.h>

#include <vector>
#include <boost/shared_ptr.hpp>

#include "../includes/rep.h"
#include "../includes/in_order.h"
#include "../includes/simple_cb.h"
#include "../includes/ll.h"
#include "../includes/cycle_timing.h"
#include "../includes/common.h"

using namespace std;

#define NUM_ARGS 2

typedef enum {LL_IMP, IN_ORDER_IMP, SIMPLE_CB_IMP} imp_t;

// order in which implementations will be run
int imps[] = {SIMPLE_CB_IMP, IN_ORDER_IMP, LL_IMP};
//int imps[] = {SIMPLE_CB_IMP};

#define TIMEBUF_SZ 80
char cur_time[TIMEBUF_SZ];

// passed in at runtime
char *cache_file_name;
char *query_file_name;

FILE *cache = NULL;
FILE *query = NULL;

typedef shared_ptr<rep> rep_ptr;

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
    simple_cb *i = new simple_cb(cur_time);
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
  for (unsigned j = UINT_MAX; fscanf(cache, "%u %hhu", &bytenum, &byteval) != EOF; j = bytenum) {
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
  char *buf = NULL;

  size_t len;
  ssize_t r = getline(&buf, &len, query);
  assert(r > 1); // must have read more than "\n"
  assert(len > 0);

  // get the last bytenum
  unsigned i = r - 1;
  while(i >= 0 && buf[i] != ' ') {i--;}
  while(i >= 0 && buf[i] == ' ') {i--;}
  while(i >= 0 && buf[i] != ' ') {i--;}
  while(i >= 0 && buf[i] == ' ') {i--;}
  while(i >= 0 && buf[i] != ' ') {i--;}
  
  char *line = &buf[i + 1];
  unsigned bytenum, byteval;
  int pos;
  int matches = sscanf(line, "%u %u%n", &bytenum, &byteval, &pos);
  assert(matches == 2);

  unsigned bv_len = bytenum + 1;
  uint8_t bv[bv_len];

  unsigned expected_bytenum;
  unsigned query_count = 0;
  do {
    assert(len > 0);
    assert(r > 1);

    expected_bytenum = 0;
    line = buf;
    while ((matches = sscanf(line, "%u %u%n", &bytenum, &byteval, &pos)) == 2) {

      assert(expected_bytenum == bytenum);
      bv[bytenum] = byteval;
      //printf("%u %u\n", bytenum, byteval);

      expected_bytenum++;
      line += pos;
    }

    printf("\r[%u]", query_count);
    fflush(stdout);
    query_count++;
    rpt->query(bv, bv_len);
    // TODO verify correctess of hits/misses against LL implementation

    // ensure that the datafile contains pairs of numbers (bytenum and byteval)
    assert(matches == -1);
  } while ((r = getline(&buf, &len, query)) != -1);

  printf("\n");

  free(buf);
}

int main(int argc, char **argv) {
  if (argc != NUM_ARGS + 1) {
    printf("Usage: %s [cache_file] [query_file]\n", argv[0]);
    return 1;
  }
  
  cache_file_name = argv[1];
  query_file_name = argv[2];

  get_str_time(cur_time, TIMEBUF_SZ);

  // TODO do we need to call this right before we time?
  int64_t func_call_overhead = warmup_time();
  (void) func_call_overhead;

  unsigned first_hits = 0, first_misses = 0;
  for(int i = 0; i < (int) (sizeof(imps) / sizeof(int)); i++) {
    printf("opening cache file\n");
    cache = fopen(cache_file_name, "r");
    if (!cache) {
      printf("Could not open cache_file \"%s\".\n", cache_file_name);
      return 3;
    }
    
    printf("opening query file\n");
    query = fopen(query_file_name, "r");
    if (!query) {
      fclose(cache);
      printf("Could not open query_file \"%s\": %s\n", query_file_name, strerror(errno));
      return 4;
    }

    printf("initializing rep %d\n", i);
    rep_ptr r = initialize_rep(imps[i]);
    assert(r);

    printf("loading rep %d\n", i);
    load(r);

    printf("preparing to query rep %d\n", i);
    r->prepare_to_query();
    
    printf("running rep %d\n", i);
    run(r);

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
    fclose(cache);

    printf("closing query file %d\n", i);
    fclose(query);
    
  }

  return 0;
}
