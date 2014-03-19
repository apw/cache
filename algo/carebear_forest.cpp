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
  
  // PARALLEL QUERY
  /*
  int err;
  
  err = pthread_mutex_init(&done_lock, NULL);
  assert(err == 0);
  
  err = pthread_cond_init(&done_cv, NULL);
  assert(err == 0);
  
  result_ = (struct helper_args *) malloc(sizeof(struct helper_args));
  assert(result_ != NULL);
  
  cancel_version_ = 1;
  */
}

carebear_forest::~carebear_forest() {
  // PARALLEL QUERY
  /*
  int err;
  
  err = pthread_mutex_destroy(&done_lock);
  assert(err == 0);
  
  err = pthread_cond_destroy(&done_cv);
  assert(err == 0);
  
  free(result_);
  */
  
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
      d->extend(*v_iter, max_bytenum, INVALID_ID);
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
    
    cout << "Trie " << num_trie << " has been constructed with " << (vects_left - done->get_size()) << " vectors." << endl;
    
    num_trie++;
  }
  done->end_trans();
  
  delete u;
  delete done;
  delete bytenums_left;
}

// PARALLEL QUERY
/*
void *do_query_helper(void *arg) {
  struct helper_args *ret = (struct helper_args *) arg;
  d_trie *d = ret->d;
  uint8_t *bv = ret->bv;
  carebear_forest *cur = ret->cur;
  
  assert(d != NULL);
  
  while(d != NULL) {
    // record number of steps
    ret->steps++;

    // if hit...
    if (d->get_bytenum() == INVALID_BYTENUM && d->get_id() != INVALID_ID) {
      assert(d->is_leaf());
      ret->res = d->get_id();
      
      pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
      pthread_mutex_lock(&cur->done_lock);
      // write changes to cur->result_ only if cancel_versions match
      if (cur->cancel_version_ == ret->cancel_version
	  && cur->result_->res == INVALID_ID) {
	*(cur->result_) = *ret;
      }

      // report another thread finished
      cur->num_finished_++;
      pthread_cond_signal(&cur->done_cv);
      pthread_mutex_unlock(&cur->done_lock);
      pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
      
      free(arg);
      return NULL;
    }
    
    uint8_t byteval = bv[d->get_bytenum()];
    d = d->decide(byteval);
  }

  pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
  pthread_mutex_lock(&cur->done_lock);
  // write changes to cur->result_ only if cancel_versions match
  if (cur->cancel_version_ == ret->cancel_version &&
      cur->result_->res == INVALID_ID && cur->result_->steps < ret->steps) {
    *(cur->result_) = *(ret);
  }
  
  // report another thread finished
  cur->num_finished_++;
  pthread_cond_signal(&cur->done_cv);
  pthread_mutex_unlock(&cur->done_lock);
  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
  
  free(arg);
  return NULL;
}
*/

unsigned carebear_forest::do_query(uint8_t *bv, unsigned len) {
  // SERIAL QUERY
  unsigned num_tries = forest_.size();
  unsigned steps;
  unsigned max_steps = 0;
  for(unsigned i = 0; i < num_tries; i++) {
    d_trie *cur = forest_[i];
    steps = 0;
    do {
      steps++;
      
      if (cur->is_leaf()) {
	assert(cur->get_bytenum() == INVALID_BYTENUM);
	num_steps_ = steps;
	return cur->get_id();
      }
      
      uint8_t byteval = bv[cur->get_bytenum()];
      cur = cur->decide(byteval);
    } while (cur != NULL);
    
    if (steps > max_steps) {
      max_steps = steps;
    }
  }
  
  num_steps_ = max_steps;
  return INVALID_ID;
}

/*
unsigned carebear_forest::do_query(uint8_t *bv, unsigned len) {
  // PARALLEL QUERY
  int err;
  
  num_finished_ = 0;
  result_->res = INVALID_ID;
  result_->steps = 0;
  
  unsigned num_tries = forest_.size();
  
  pthread_t threads[num_tries];
  struct helper_args *args[num_tries];
  
  // lock to ensure no signals go off before we go to sleep
  pthread_mutex_lock(&done_lock);
  
  // initialize query threads
  for(unsigned i = 0; i < num_tries; i++) {
    // args[i] shoudl be freed by its corresponding worker query thread
    args[i] = (struct helper_args *) malloc(sizeof(struct helper_args));
    assert(args[i] != NULL);
    
    args[i]->cur = this;
    args[i]->d = forest_[i];
    args[i]->bv = bv;
    args[i]->steps = 0;
    args[i]->res = INVALID_ID;
    args[i]->cancel_version = cancel_version_;
    
    pthread_attr_t attr;
    err = pthread_attr_init(&attr);
    assert(err == 0);
    
    err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    assert(err == 0);
    
    err = pthread_create(&threads[i], &attr,
			 do_query_helper,
			 (void *) args[i]);
    assert(err == 0);
    
    err = pthread_attr_destroy(&attr);
    assert(err == 0);
  }
  
  unsigned miss_max_steps = 0;
  while(num_finished_ < num_tries) {
    err = pthread_cond_wait(&done_cv, &done_lock);
    assert(err == 0);
        
    unsigned id = result_->res;
    
    // if hit
    if (id != INVALID_ID) {
      // report number of steps to super::rep
      num_steps_ = result_->steps;
      
      // ensure no worker query thread writes to result_
      cancel_version_++;
      
      // ensure no integer overflow
      assert(cancel_version_ > 0);
      pthread_mutex_unlock(&done_lock);

      // kill all threads
      for(unsigned i = 0; i < num_tries; i++) {
	pthread_cancel(threads[i]);
      }
      
      return id;
    } else {
      // if miss number of steps should be maximum of number of steps across
      // all tries
      if (result_->steps > miss_max_steps) {
	miss_max_steps = result_->steps;
      }
    }
  }

  // ensure no worker query thread writes to result_
  cancel_version_++;  
  
  // ensure no integer overflow
  assert(cancel_version_ > 0);
  pthread_mutex_unlock(&done_lock);
  
  assert(miss_max_steps > 0);

  // report number of steps to super::rep
  num_steps_ = miss_max_steps;
  
  // return miss
  return INVALID_ID;
}
*/

void carebear_forest::viz() {
  char *dot_filename = (char *) calloc(BUFLEN, sizeof(char));
  assert(dot_filename);
  
  snprintf(dot_filename, sizeof(char) * BUFLEN, "viz/%s.dot", outfile_basename_);

  ofstream outfile;
  outfile.open(dot_filename);
  
  outfile << "digraph {" << endl;
  
  unsigned cur_id = 0;
  unsigned next_id = 1;
  for(unsigned i = 0; i < forest_.size(); i++) {
    forest_[i]->graph_to_ofstream(outfile, cur_id, &next_id);
    cur_id = next_id;
    next_id++;
  }
  
  outfile << "}";
  outfile.close();
  
  free(dot_filename);
}
