#include "../includes/dgen.h"
#include "../includes/rep.h"
#include "../includes/d_trie.h"

#include <stdlib.h>
#include <assert.h>
#include <errno.h>

#include <iostream>
#include <random>
#include <vector>
#include <chrono>

using namespace std;

default_random_engine get_seeded_generator() {
  unsigned seed = chrono::system_clock::now().time_since_epoch().count();
  default_random_engine g(seed);
  return g;  
}

static void unif_no_rep(unsigned min, unsigned max,
			unsigned *nums, unsigned n) {
  assert(n > 0);
  assert(min < max);
  assert(max - min >= n); // not correct...are max and min inclusive or exclusive? if inclusive, then should be (max-min >= n-1); if exclusive, then should be (max-min > n); if first inclusive and second exclusive, then is fine but should be documented in comments
  
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

void gen_opt_dtrie(struct cache_params *cp, d_trie *solution, cache *c) {
  assert(solution == NULL);
  assert(cp != NULL);
  
  assert(cp->num_vects > 0);
  assert(cp->vect_len > 0);
  assert(0 < cp->m_num_rel && cp->m_num_rel <= 1);
  assert(0 <= cp->std_num_rel && cp->std_num_rel <= 1);
  
  vect *s = (vect *) malloc(sizeof(vect) * cp->num_vects);
  assert(s != NULL);
  
  // create the cache
  unsigned num_rel;
  for(unsigned i = 0; i < cp->num_vects; i++) {
    default_random_engine rel_g = get_seeded_generator();
    normal_distribution<float> rel_dist(cp->m_num_rel * cp->vect_len,
						cp->std_num_rel * cp->vect_len);
    do {
      num_rel = rel_dist(rel_g);
    } while (num_rel > cp->vect_len);
    
    unsigned *rel_bytenums = (unsigned *) malloc(sizeof(unsigned) * num_rel);
    assert(rel_bytenums != NULL);
    unif_no_rep(0, cp->vect_len, rel_bytenums, num_rel);
    // note: the picked bytenums are not sorted and that is
    // crucial for the next part, in which the d_trie is generated.
    
    default_random_engine val_g = get_seeded_generator();
    uniform_int_distribution<uint8_t> val_dist(0, ((uint8_t) -1));
    
    for(unsigned j = 0; j < num_rel; j++) {
      unsigned bytenum = rel_bytenums[j];
      uint8_t byteval = val_dist(val_g);
      
      entry e;
      e.bytenum = bytenum;
      e.byteval = byteval;
      s[i].push_back(e);
      
      if (c->find(bytenum) == c->end()) {
	bytenum_set bs;
	c->operator[](bytenum) = bs;
      }
      
      c->operator[](bytenum)[j] = byteval;
    }
    
    free(rel_bytenums);
  }
  
  // put the cache in the d_trie
  solution = new d_trie(s[0][0].bytenum, 0);
  d_trie *iter = solution;
  for(unsigned j = 0; j < s[0].size() - 1; j++) {
    iter->extend(s[0][j].byteval, s[0][j + 1].bytenum, 0);
    iter = iter->decide(s[0][j].byteval);
  }  
  iter->extend(s[0][s[0].size() - 1].byteval, INVALID_BYTENUM, 0);
  
  for(unsigned i = 1; i < cp->num_vects; i++) {
    iter = solution;
    
    for(unsigned j = 0; j < s[i].size() - 1; j++) {
      if (iter->is_leaf()) {
	iter->extend(s[i][j].byteval, s[i][j + 1].bytenum, i);
      } else {
	assert(c->find(s[i][j].bytenum) != c->end());
	if (c->operator[](s[i][j].bytenum).find(i) 
	    != c->operator[](s[i][j].bytenum).end()) {
	  iter->extend(s[i][j].byteval, s[i][j + 1].bytenum, i);
	} else {
	  // TODO ahhh
	}
      }
      
      iter = iter->decide(s[i][j].byteval);
    }
    
    iter->extend(s[i][s[i].size() - 1].byteval, INVALID_BYTENUM, i);    
  }

  free(s);
}

int main(int argc, char **argv) {
  // get arguments n stuff
  struct cache_params cp;
  cp.num_vects = 10;
  cp.vect_len = 10;
  cp.m_num_rel = 0.5;
  cp.std_num_rel = 0.01;

  d_trie *solution = NULL;
  cache c;
  gen_opt_dtrie(&cp, solution, &c);
  assert(solution != NULL);
  
  // generate cache vectors
  // assign absorption rates
  // generate query vectors
  // dun forget to output vectors to target files
  
  delete solution;
  
  return 0;
}
