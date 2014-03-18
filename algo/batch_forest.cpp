#include "../includes/rep.h"
#include "../includes/in_order.h"
#include "../includes/batch_forest.h"
#include "../includes/common.h"
#include "../includes/common_types.h"

#include <iostream>
#include <vector>
#include <algorithm>

#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <pthread.h>

batch_forest::batch_forest(const char *cur_time) 
  : rep(cur_time, "batch_forest"), carebear_forest(cur_time) {
  
}

void batch_forest::prepare_to_query() {
  batches b;
  
  for(unsigned i = 0, n = s_.size(); i < n; i++) {
    bool need_new_batch = true;
    
    // see if vector s_[i] is in any of the existing batches
    for (unsigned j = 0, num_batches = b.size(); j < num_batches; j++) {
      c_entry batch_vect = b[j][0];
      bool match;
      
      unsigned l1 = s_[i].size(), l2 = batch_vect.ve.size();
      if (l1 == l2) {
	match = true;
	
	// compare the vector's cares against the cares of the first vector in the set
	for(unsigned k = 0; k < l1 && k < l2; k++) {
	  // no match
	  if (batch_vect.ve[k].bytenum != s_[i][k].bytenum
	      || batch_vect.ve[k].byteval != s_[i][k].byteval) {
	    match = false;
	    break;
	  }
	}	
      } else {
	// vectors are not equal because they have different lengths
	match = false;
      }

      if (match) {
	c_entry v;
	v.ve = s_[i];
	v.id = i;
	
	b[j].push_back(v);
	need_new_batch = false;
	break;
      }
    }
    
    // vector did not fit in anywhere so needs a new batch
    if (need_new_batch) {
      c_entry v;
      v.ve = s_[i];
      v.id = i;

      vector<c_entry> new_batch;
      new_batch.push_back(v);
      
      b.push_back(new_batch);
    }
  }

  // create a forest for each batch
  construct_forest(&b);
}

int batch_forest::get_max_bytenum(uset_uint *done, uset_uint *u,
				uset_uint *bytenums_left,
				uset_uint *batch_vectors) {
  unsigned max_variability = 0;
  unsigned max_num_care = 0;
  int max_bytenum = -1;
  cache::const_iterator c_end = c_.end();
  for(cache::const_iterator c_iter = c_.begin(); c_iter != c_end; c_iter++) {
    if (bytenums_left->lookup(c_iter->first)) {
      unsigned cur_num_care = 0;
      tr1::unordered_set<uint8_t> val_set;
      bytenum_set::const_iterator b_end = c_[c_iter->first].end();
      for(bytenum_set::const_iterator b_iter = c_[c_iter->first].begin(); 
	  b_iter != b_end; b_iter++) {

	if (done->lookup(b_iter->first)
	    && u->lookup(b_iter->first)
	    && batch_vectors->lookup(b_iter->first)) {
	  
	  cur_num_care++;
	  
	  if (val_set.count(b_iter->second) == 0) {
	    val_set.insert(b_iter->second);
	  }
	}
      }

      unsigned cur_variability = val_set.size();
      if (cur_variability > max_variability 
	  || (cur_variability == max_variability && cur_num_care > max_num_care)) {
	max_variability = cur_variability;
	max_bytenum = c_iter->first;
	max_num_care = cur_num_care;
      }
    }
  }
  
  if (max_bytenum != -1) {
    bytenums_left->begin_trans();
    bytenums_left->remove(max_bytenum);
    bytenums_left->end_trans();
  }
  
  return max_bytenum;
}

void batch_forest::construct_forest(batches *b) {
  // find max relevant bytenum in tera
  max_relevant_bytenum_ = 0;
  cache::const_iterator c_end = c_.end();
  for(cache::const_iterator c_iter = c_.begin(); c_iter != c_end; c_iter++) {
    if (c_iter->first > max_relevant_bytenum_) {
      max_relevant_bytenum_ = c_iter->first;
    }
  }

  uset_uint *done = new uset_uint(current_id_ + 1);
  uset_uint *u = new uset_uint(current_id_ + 1);
  uset_uint *bytenums_left = new uset_uint(max_relevant_bytenum_ + 1);
  uset_uint *batch_vectors = new uset_uint(current_id_ + 1);
  
  unsigned num_trie = 0;
  unsigned vects_left;
  
  done->begin_trans();
  while(done->get_size() > 0) {
    vects_left = done->get_size();
    
    // remove all bytenums that are not in current batch from batch_vectors
    batch_vectors->begin_trans();
    vect v = b->operator[](num_trie)[0].ve;
    unsigned last = 0;
    // iterate through bytenums of first vector in the batch
    for(unsigned i = 0, n = v.size(); i < n; i++) {
      assert(last <= i);
      
      // remove all vectors between 'last' and 'i - 1'
      if (last < v[i].bytenum) {
	for(unsigned j = last; j < i; j++) {
	  batch_vectors->remove(i);
	}
      }
      
      last = i + 1;
    }
    batch_vectors->end_trans();
    
    // find the bytenum with the largest number of don't care
    assert(u->get_size() == u->get_capacity());
    int max_bytenum = get_max_bytenum(done, u, bytenums_left, batch_vectors);
    if (max_bytenum == -1) {
      assert(0);
    }
    
    // make the max_bytenum the question at the top of the subtrie
    d_trie *d = new d_trie(max_bytenum, INVALID_ID);
    forest_.push_back(d);
  
    populate_subtrie(d, done, u, bytenums_left);
  
    // undo trans made by get_max_bytenum
    bytenums_left->undo_trans();
    
    // clean slate for next batch
    batch_vectors->undo_trans();
    
    cout << "Trie " << num_trie << " has been constructed with " << (vects_left - done->get_size()) << " vectors." << endl;
    
    num_trie++;
  }
  done->end_trans();
  
  delete u;
  delete done;
  delete bytenums_left;
  delete batch_vectors;
}
