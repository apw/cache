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
#include "../includes/ll.h"
#include "../includes/cycle_timing.h"
#include "../includes/common.h"

using namespace std;

#define NUM_ARGS 2

typedef enum {LL_IMP, IN_ORDER_IMP} imp_t;

// order in which implementations will be run
int imps[] = {LL_IMP, IN_ORDER_IMP};

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
  char *buf = NULL;

  int id = 0;
  size_t len;
  ssize_t r;
  while ((r = getline(&buf, &len, cache)) != -1) {
    assert(!ferror(cache));
    assert(len > 0);

    rp->begin_sbv(id);

    char *line = buf;
    unsigned bytenum, byteval;
    int matches, pos;
    while ((matches = sscanf(line, "%u %u%n", &bytenum, &byteval, &pos)) == 2) {
      assert(!ferror(cache));

      rp->add_byte(id, bytenum, byteval);
      line += pos;
    }

    // ensure that the datafile contains pairs of numbers (bytenum and byteval)
    assert(matches == -1);

    rp->end_sbv(id);
    id++;
  }

  free(buf);
  assert(feof(cache));
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
  int hit;
  (void) hit;
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
    
    hit = rpt->query(bv, bv_len);
    // TODO verify correctess of hits/misses against LL implementation

    // ensure that the datafile contains pairs of numbers (bytenum and byteval)
    assert(matches == -1);
  } while ((r = getline(&buf, &len, query)) != -1);

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

  for(int i = 0; i < (int) (sizeof(imps) / sizeof(int)); i++) {
    cache = fopen(cache_file_name, "r");
    if (!cache) {
      printf("Could not open cache_file \"%s\".\n", cache_file_name);
      return 3;
    }
    
    query = fopen(query_file_name, "r");
    if (!query) {
      fclose(cache);
      printf("Could not open query_file \"%s\": %s\n", query_file_name, strerror(errno));
      return 4;
    }

    rep_ptr r = initialize_rep(i);
    assert(r);

    load(r);
    r->prepare_to_query();
    run(r);
    cleanup(r);
    
    fclose(cache);
    fclose(query);
  }

  return 0;
}
