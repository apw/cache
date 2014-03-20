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
	  // verify bytenums are always in increasing order
	  if (k > 0) {
	    assert(s_[i][k].bytenum > s_[i][k - 1].bytenum);
	  }
	  
	  // no match
	  if (batch_vect.ve[k].bytenum != s_[i][k].bytenum) {
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

  cout << b.size() << " batches constructed."<< endl;
  
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

void batch_forest::populate_forest_trie(d_trie *d, uset_uint *done,
				       uset_uint *u, uset_uint *bytenums_left,
				       uset_uint *batch_vectors) {
  // remove all vectors that have a don't-care at d->get_bytenum() from u
  u->begin_trans();
  for(unsigned i = 0, n = done->get_capacity(); i < n; i++) {
    if (done->lookup(i) && u->lookup(i) && batch_vectors->lookup(i)
	&& c_[d->get_bytenum()].count(i) == 0) {
      u->remove(i);
    }
  }
  u->end_trans();

  // Beginning of stupid Theta(n^2)
  // of all remaining candidate vectors, find all unique vals at d->get_bytenum()
  tr1::unordered_set<uint8_t> val_set;
  bytenum_set::const_iterator b_end = c_[d->get_bytenum()].end();
  for(bytenum_set::const_iterator b_iter = c_[d->get_bytenum()].begin(); 
      b_iter != b_end; b_iter++) {
    // first is id; second is byteval
    if (done->lookup(b_iter->first) && u->lookup(b_iter->first)
	&& batch_vectors->lookup(b_iter->first) && val_set.count(b_iter->second) == 0) {
      val_set.insert(b_iter->second);      
    }
  }

  for (tr1::unordered_set<uint8_t>::const_iterator v_iter = val_set.begin();
       v_iter != val_set.end(); v_iter++) {
    // remove all vectors that don't have the current byteval at the bytenum
    u->begin_trans();
    bytenum_set::const_iterator b_end = c_[d->get_bytenum()].end();
    for(bytenum_set::const_iterator b_iter = c_[d->get_bytenum()].begin(); 
	b_iter != b_end; b_iter++) {
      if (done->lookup(b_iter->first) && u->lookup(b_iter->first)
	  && batch_vectors->lookup(b_iter->first) && b_iter->second != *v_iter) {
	u->remove(b_iter->first);
      }
    }
    u->end_trans();
    
    // find next bytenum with highest utility
    int max_bytenum = get_max_bytenum(done, u, bytenums_left, batch_vectors);
    if (max_bytenum == -1) {
      for(unsigned i = 0, n = done->get_capacity(); i < n; i++) {
	if (done->lookup(i) && u->lookup(i) && batch_vectors->lookup(i)) {
	  // 'i' is the id of the vector that we are about to finish inserting
	  d->extend(c_[d->get_bytenum()][i], INVALID_BYTENUM, i);
	  done->remove(i);
	}
      }
    } else {    
      // extend the trie!
      d->extend(*v_iter, max_bytenum, INVALID_ID);
      d_trie *child = d->decide(*v_iter);
      
      // ...and now recurse down!
      populate_forest_trie(child, done, u, bytenums_left, batch_vectors);
      
      // undo trans performed in get_max_bytenum
      bytenums_left->undo_trans();
    }
    
    // undo trans
    u->undo_trans();    
  }
  
  // End of stupid Theta(n^2)

  // undo removal
  u->undo_trans();
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
  
  unsigned vects_left;
  
  done->begin_trans();
  for(unsigned num_trie = 0; num_trie < b->size(); num_trie++) {
    cout << "Constructing Trie for Batch " << num_trie << " with " << b->operator[](num_trie).size() << " vectors." << endl;
    
    assert(done->get_size() > 0);
    vects_left = done->get_size();
    
    // remove all vectors that are not in current batch from batch_vectors
    batch_vectors->begin_trans();
    for(unsigned i = 0; i < b->size(); i++) {
      // if 'i' refers to current batch, skip it
      if (i == num_trie) {
	continue;
      }
      
      // otherwise, remove everything in the batch from the done set
      vector<c_entry> cur_batch = b->operator[](i);
      for(unsigned j = 0; j < cur_batch.size(); j++) {
	batch_vectors->remove(cur_batch[j].id);
      }
    }
    batch_vectors->end_trans();
    
    // find the bytenum with the largest number of don't care
    assert(u->get_size() == u->get_capacity());
    int max_bytenum = get_max_bytenum(done, u, bytenums_left, batch_vectors);
    if (max_bytenum == -1) {
      assert(0);
    }
    
    // make the max_bytenum the question at the top of the forest_trie
    d_trie *d = new d_trie(max_bytenum, INVALID_ID);
    forest_.push_back(d);
    
    populate_forest_trie(d, done, u, bytenums_left, batch_vectors);
  
    // undo trans made by get_max_bytenum
    bytenums_left->undo_trans();
    
    // clean slate for next batch
    batch_vectors->undo_trans();
    
    unsigned batch_size = b->operator[](num_trie).size();
    assert(batch_size == vects_left - done->get_size());
  }
  assert(done->get_size() == 0);
  done->end_trans();
  
  delete u;
  delete done;
  delete bytenums_left;
  delete batch_vectors;
}
