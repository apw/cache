#include "../includes/rep.h"
#include "../includes/in_order.h"
#include "../includes/carebear_forest.h"
#include "../includes/common.h"

#include <iostream>
#include <vector>
#include <algorithm>

#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <pthread.h>

carebear_forest::carebear_forest(const char *cur_time) 
  : rep(cur_time, "carebear_forest"), in_order(cur_time) {
  
}

carebear_forest::~carebear_forest() {
  for(unsigned i = 0; i < forest_.size(); i++) {
    delete forest_[i];
  }
}

int carebear_forest::get_max_bytenum(uset_uint *done, uset_uint *u, uset_uint *bytenums_left) {
  unsigned max_num_care = 0;
  int max_bytenum = -1;
  cache::const_iterator c_end = c_.end();
  for(cache::const_iterator c_iter = c_.begin(); c_iter != c_end; c_iter++) {
    
    if (bytenums_left->lookup(c_iter->first)) {
      unsigned cur_num_care = 0;
      bytenum_set::const_iterator b_end = c_[c_iter->first].end();
      for(bytenum_set::const_iterator b_iter = c_[c_iter->first].begin(); 
	  b_iter != b_end; b_iter++) {
	if (done->lookup(b_iter->first) && u->lookup(b_iter->first)) {
	  cur_num_care++;
	}
      }
      
      if (cur_num_care > max_num_care) {
	max_num_care = cur_num_care;
	max_bytenum = c_iter->first;
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

void carebear_forest::populate_subtrie(d_trie *d, uset_uint *done,
				       uset_uint *u, uset_uint *bytenums_left) {
  // remove all vectors that have a don't-care at d->get_bytenum() from u

  u->begin_trans();
  for(unsigned i = 0, n = done->get_capacity(); i < n; i++) {
    if (done->lookup(i) && u->lookup(i) 
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
	&& val_set.count(b_iter->second) == 0) {
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
	  && b_iter->second != *v_iter) {
	u->remove(b_iter->first);
      }
    }
    u->end_trans();
    
    // find next bytenum with highest utility
    int max_bytenum = get_max_bytenum(done, u, bytenums_left);
    if (max_bytenum == -1) {
      // HIHI
      for(unsigned i = 0, n = done->get_capacity(); i < n; i++) {
	if (done->lookup(i) && u->lookup(i)) {
	  // 'i' is the id of the vector that we are about to finish inserting
	  d->extend(c_[d->get_bytenum()][i], INVALID_BYTENUM, i);
	  done->remove(i);
	  break;
	}
      }
    } else {    
      // extend the trie!
      d->extend(*v_iter, max_bytenum, INVALID_BYTENUM);
      d_trie *child = d->decide(*v_iter);
      
      // ...and now recurse down!
      populate_subtrie(child, done, u, bytenums_left);
      
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

void carebear_forest::prepare_to_query() {
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
  
  unsigned num_trie = 0;
  unsigned vects_left;
  
  done->begin_trans();
  while(done->get_size() > 0) {
    vects_left = done->get_size();
    
    // find the bytenum with the largest number of don't care
    assert(u->get_size() == u->get_capacity());
    int max_bytenum = get_max_bytenum(done, u, bytenums_left);
    if (max_bytenum == -1) {
      assert(0);
    }
    
    // make the max_bytenum the question at the top of the subtrie
    d_trie *d = new d_trie(max_bytenum, INVALID_ID);
    forest_.push_back(d);
  
    populate_subtrie(d, done, u, bytenums_left);
  
    // undo trans made by get_max_bytenum
    bytenums_left->undo_trans();
    
    cout << "Trie " << num_trie << " constructed with " << (vects_left - done->get_size()) << " vectors." << endl;
    
    num_trie++;
  }
  done->end_trans();
  
  delete u;
  delete done;
  delete bytenums_left;
}

void *do_query_helper(void *arg) {
  struct helper_args *ret = (struct helper_args *) arg;
  d_trie *d = ret->d;
  uint8_t *bv = ret->bv;
  carebear_forest *cur = ret->cur;
  
  while(d != NULL) {
    ret->steps++;

    if (d->get_bytenum() == INVALID_BYTENUM && d->get_id() != INVALID_ID) {
      assert(d->is_leaf());
      ret->res = d->get_id();
      
      pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
      pthread_mutex_lock(&cur->done_lock);
      cur->set_result(ret);
      pthread_cond_signal(&cur->done_cv);
      pthread_mutex_unlock(&cur->done_lock);
      pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
      return NULL;
    }
    
    uint8_t byteval = bv[d->get_bytenum()];
    d = d->decide(byteval);
  }

  pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
  pthread_mutex_lock(&cur->done_lock);
  cur->set_result(ret);
  pthread_cond_signal(&cur->done_cv);
  pthread_mutex_unlock(&cur->done_lock);
  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
  return NULL;
}

void carebear_forest::set_result(struct helper_args *r) {
  result_ = r;
}

unsigned carebear_forest::do_query(uint8_t *bv, unsigned len) {
  int err;
  
  unsigned num_tries = forest_.size();
  
  pthread_t threads[num_tries];
  struct helper_args args[num_tries];
  
  pthread_mutex_lock(&done_lock);
  
  for(unsigned i = 0; i < num_tries; i++) {
    args[i].cur = this;
    args[i].d = forest_[i];
    args[i].bv = bv;
    args[i].steps = 0;
    args[i].res = INVALID_ID;
    
    err = pthread_create(&threads[i], NULL,
			 do_query_helper,
			 (void *) &args[i]);

    assert(err == 0);
  }
  
  unsigned num_done = 0;
  while(num_done < num_tries) {
    err = pthread_cond_wait(&done_cv, &done_lock);
    assert(err == 0);

    num_steps_ = result_->steps;
    unsigned id = result_->res;
    
    // a thread signaled to us that it finished
    if (id != INVALID_ID) {
      pthread_mutex_unlock(&done_lock);
      
      // kill all threads
      for(unsigned i = 0; i < num_tries; i++) {
	pthread_cancel(threads[i]);
      }
      
      // return hit
      return id;
    }
    
    num_done++;
  }
  
  pthread_mutex_unlock(&done_lock);
  
  // return miss
  return INVALID_ID;
}
