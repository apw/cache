#include "../includes/dgen.h"
#include "../includes/rep.h"

#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <iostream>
#include <random>
#include <vector>
#include <chrono>
#include <algorithm>

using namespace std;

default_random_engine get_seeded_generator() {
  unsigned seed = chrono::system_clock::now().time_since_epoch().count();
  default_random_engine g(seed);
  return g;  
}

// numbers in: [min, max)
static void unif_no_rep(unsigned min, unsigned max,
			unsigned *nums, unsigned n) {
  assert(n > 0);
  assert(min < max);
  assert(max - min >= n);
  
  unsigned range = max - min;
  unsigned perm[range];
  for(unsigned i = 0; i < range; i++) {
    perm[i] = i + min;
  }

  default_random_engine g = get_seeded_generator();
  uniform_int_distribution<unsigned> dist(0, range - 1);
  
  // fisher-yates shuffle
  unsigned r, tmp;
  for(unsigned i = 0; i < range; i++) {
    r = dist(g);
    
    tmp = perm[i];
    perm[i] = perm[r];
    perm[r] = tmp;
  }
  
  // return array of unif random unsigned ints
  for(unsigned i = 0; i < n; i++) {
    nums[i] = perm[i];
  }
}

bool entryCmp(entry a, entry b) {
  return a.bytenum < b.bytenum;
}

void gen_cache(struct cache_params *cp, tmp_cache_rep *c) {
  assert(cp != NULL);
  
  assert(cp->num_vects > 0);
  assert(cp->vect_len > 0);
  assert(0 < cp->m_num_rel && cp->m_num_rel <= 1);
  assert(0 <= cp->std_num_rel && cp->std_num_rel <= 1);
  
  vect *s = (vect *) malloc(sizeof(vect) * cp->num_vects);
  assert(s != NULL);
  
  // iterate over vectors that will be generated
  unsigned num_rel;
  for(unsigned i = 0; i < cp->num_vects; i++) {
    // create distribution for generating the number of relevant bytenums of 
    // this vector
    default_random_engine rel_g = get_seeded_generator();
    normal_distribution<float> rel_dist(cp->m_num_rel * cp->vect_len,
						cp->std_num_rel * cp->vect_len);
    
    // loop to ensure that number of relevant bytenums is not greater than max
    // vector length
    do {
      num_rel = rel_dist(rel_g);
    } while (num_rel > cp->vect_len);
    
    // pick which bytenums will be relevant uniformly
    unsigned *rel_bytenums = (unsigned *) malloc(sizeof(unsigned) * num_rel);
    assert(rel_bytenums != NULL);
    unif_no_rep(0, cp->vect_len, rel_bytenums, num_rel);
    // note: the picked bytenums are not sorted
    
    // create distribution for generating bytevals
    default_random_engine val_g = get_seeded_generator();
    uniform_int_distribution<uint8_t> val_dist(0, ((uint8_t) -1));
    
    // iterate over relevant bytenums and pick bytevals for them ~ Unif(0, 255)
    tmp_cache_entry line;
    for(unsigned j = 0; j < num_rel; j++) {
      unsigned bytenum = rel_bytenums[j];
      uint8_t byteval = val_dist(val_g);
      
      // this now puts 'bytenum' and 'byteval' in teravectyl, TODO put this in an array of vectors
      // that will be passed into this function from main instead of 'cache c'
      entry e;
      e.bytenum = bytenum;
      e.byteval = byteval;
      line.push_back(e);

      /*
      entry e;
      e.bytenum = bytenum;
      e.byteval = byteval;
      s[i].push_back(e);
      
      if (c->find(bytenum) == c->end()) {
	bytenum_set bs;
	c->operator[](bytenum) = bs;
      }
      
      c->operator[](bytenum)[j] = byteval;
      */
    }
    
    c->push_back(line);

    free(rel_bytenums);
  }

  // sort the entries in each line in the cache in bytenum order
  unsigned int siz = c->size();
  for (unsigned int i = 0; i < siz; i++) {
    std::sort(c->operator[](i).begin(),
	      c->operator[](i).end(),
	      entryCmp);
  }
  
  free(s);
}

// returns true iff q is a hit in c
bool is_hit(tmp_query q, tmp_cache_rep c) {
  unsigned i, j, c_size, l_size, num;
  uint8_t val;
  bool hit;
  // iterate over cache-vectors looking for a hit
  c_size = c.size();
  for (i = 0; i < c_size; i++) {
    // iterate over cache-vector-bytenums
    l_size = c[0].size();
    hit = true;
    for (j = 0; j < l_size; j++) {
      num = c[i][j].bytenum;
      val = c[i][j].byteval;
      if (q[num].byteval != val) {
	// found a conflicting byte; break out of inner loop with
	// hit = false so outer loop keeps going if possible
	hit = false;
	break;
      }
    }
   
    if (hit == true) {
      // found a hit
      return true;
    }
  }
  
  // iterated over all contents of c without finding a hit
  return false;
}

// cp must be the cp that was passed into gen_cache to get c
void gen_query(struct cache_params *cp,
	       struct query_params *qp,
	       tmp_cache_rep *c,
	       tmp_query_stream_rep *q) {
  unsigned i, j;
  // make sure invariants about input hold
  assert(qp);
  assert(c);
  assert(q);
  assert(qp->num_vects > 0);
  assert(qp->vect_len >= cp->vect_len);
  assert(qp->num_hits <= qp->num_vects);
  assert(cp->num_vects == c->size());

  // initialize random-stuff
  default_random_engine val_g = get_seeded_generator();
  uniform_int_distribution<uint8_t> val_dist(0, (uint8_t) -1);
  default_random_engine cache_g = get_seeded_generator();
  uniform_int_distribution<unsigned> cache_dist(0, cp->num_vects-1);

  // generate hit queries
  // iterate over hit-query vectors to be created
  entry e;
  tmp_query tmp;
  unsigned cache_index;
  unsigned num;
  uint8_t val;
  for (i = 0; i < qp->num_hits; i++) {
    // make sure tmp has no data in it
    tmp.clear();

    // iterate over bytenums in the query, assigning random values
    for (j = 0; j < qp->vect_len; j++) {
      e.bytenum = j;
      e.byteval = val_dist(val_g);
      tmp.push_back(e);
    }

    // randomly choose which cache-vector this will be a hit on
    // and put the relevant bytenums from the
    // hit-cache-vector into the hit-query-vector;
    // this should give tmp the correct relevant values at bytenums
    // relevant to this vector and random values at bytenums
    // irrelevant to this vector
    cache_index = cache_dist(cache_g);
    j = c->operator[](cache_index).size();
    for (i = 0; i < j; i++) {
      num = c->operator[](cache_index)[i].bytenum;
      val = c->operator[](cache_index)[i].byteval;
      tmp[num].bytenum = num;
      tmp[num].byteval = val;
    }
    
    q->push_back(tmp);
  }
  
  unsigned num_misses = qp->num_vects - qp->num_hits;
  for (i = 0; i < num_misses; i++) {
    cout << i << endl;
    // generate random queries until we get a miss-query
    unsigned count = 0; // !!!
    do {
      cout << count << " "; // !!!
      // make sure tmp has no data
      tmp.clear();

      // iterate over bytenums in a query, assigning random values
      for (j = 0; j < qp->vect_len; j++) {
	e.bytenum = j;
	e.byteval = val_dist(val_g);
	tmp.push_back(e);
      }
      
      count++; // !!!
    } while (!is_hit(tmp, *c)); // !!!

    q->push_back(tmp);
  }
}

int main(int argc, char **argv) {
  /*
  struct stat s;
  int err;

  if (argc < 3 || argc > 3) {
    printf("USAGE: ./bin/dgen [cache file name] [query file name]\n");
    return 0;
  }
  */

  /*
  // assert that the cache file and query file don't exist
  err = stat(argv[1], &s);
  assert(err == -1);
  assert(errno == ENOENT);
  err = stat(argv[2], &s);
  assert(err == -1);
  assert(errno == ENOENT);
  */

  // create the cache and query output files
  
  // get arguments n stuff
  struct cache_params cp;
  cp.num_vects = 10;
  cp.vect_len = 100;
  cp.m_num_rel = 0.5;
  cp.std_num_rel = 0.001;

  struct query_params qp;
  qp.num_vects = 10;
  qp.vect_len = 120;
  qp.num_hits = 8;

  // generate cache contents based on cache params
  tmp_cache_rep c;
  gen_cache(&cp, &c);
  
  cout << "CACHE CONTENTS" << endl;
  unsigned int outerIterMax, innerIterMax;
  unsigned int i, j;
  outerIterMax = c.size();
  for (i = 0; i < outerIterMax; i++) {
    innerIterMax = c[i].size();
    for (j = 0; j < innerIterMax; j++) {
      cout << c[i][j].bytenum << " " << (unsigned) c[i][j].byteval;
      if (j == innerIterMax-1) {
	cout << endl;
      } else {
	cout << " ";
      }
    }
  }
  
  // generate query contents based on query params, cache params/contents
  tmp_query_stream_rep q;
  gen_query(&cp, &qp, &c, &q);

  cout << endl << "QUERY CONTENTS" << endl;
  outerIterMax = q.size();
  for (i = 0; i < outerIterMax; i++) {
    innerIterMax = q[i].size();
    for (j = 0; j < innerIterMax; j++) {
      cout << q[i][j].bytenum << " " << (unsigned) q[i][j].byteval;
      if (j == innerIterMax-1) {
	cout << endl;
      } else {
	cout << " ";
      }
    }
  }


  // TODO ouput queries to cache and query files instead of cout
  
  return 0;
}
