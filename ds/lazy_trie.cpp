#include "../includes/rep.h"
#include "../includes/lazy_trie.h"

#include "assert.h"
#include <iostream>

lazy_trie::lazy_trie(unsigned bytenum, unsigned id, unsigned cur_index,
		     unsigned num_relevant, unsigned *relevant) {

  bytenum_ = bytenum;
  id_ = id;
  
  cur_index_ = cur_index;
  num_relevant_ = num_relevant;
  relevant_ = relevant;
  
  // node starts its life cycle lazy
  s_ = NULL;
  children_ = new offspring();
}

lazy_trie::~lazy_trie(void) {
  offspring::const_iterator children_end = children_->end();
  for(offspring::const_iterator c_iter = children_->begin(); c_iter != children_end; c_iter++) {
    delete c_iter->second;
  }
  
  if (s_ != NULL) {
    delete s_;
  }
  
  if (children_ != NULL) {
    delete children_;
  }
}

bool lazy_trie::is_lazy() {
  assert((s_ == NULL && children_ != NULL) || (s_ != NULL && children_ == NULL));
  return children_ == NULL;
}

void lazy_trie::burst() {
  assert(is_lazy());
  
  children_ = new offspring();
  for(unsigned i = 0; i < s_->size(); i++) {
    unsigned j = 0;
    for(; j < s_->operator[](i).size(); j++) {
      if (s_->operator[](i)[j].bytenum == relevant_[cur_index_ + 1]) {
	if (children_->count(s_->operator[](i)[j].byteval) == 0) {
	  lazy_trie *child = new lazy_trie(s_->operator[](i)[j].bytenum,
					   i, cur_index_ + 1,
					   num_relevant_, relevant_);
	  children_->operator[](s_->operator[](i)[j].byteval) = child;
	}
	
	children_->operator[](s_->operator[](i)[j].byteval)->add_vect(s_->operator[](i), i);
      }
    }    
  }
  
  delete s_;
  s_ = NULL;
}

lazy_trie *lazy_trie::decide(uint8_t byteval) {
  if (this->is_lazy()) {
    this->burst();
  }
  
  if (children_->count(byteval) == 0) {
    return NULL;
  }
  
  return children_->operator[](byteval);
}

void lazy_trie::add_vect(vect bv, unsigned id) {
  if (this->is_lazy()) {
    
  } else {
    
  }
  
  /*
  lazy_trie *current = this;
  for(unsigned i = 0; i < len; i++) {
    if (current->children_->count(bv[i]) > 0) {
      current = current->children_->operator[](bv[i]);
    } else {
      lazy_trie *d = new lazy_trie(i, id);
      current->children_->operator[](bv[i]) = d;
      current = d;
    }
  }
  */
  
  // TODO
}

bool lazy_trie::is_leaf() {
  return !this->is_lazy() && children_->empty();
}

unsigned lazy_trie::get_bytenum() {
    return bytenum_;
}

unsigned lazy_trie::get_id() {
  return id_;
}

void lazy_trie::print_helper(unsigned spaces) {
  string tab("\n");
  string unit("||");
  for(unsigned i = 0; i < spaces; i++) {
    tab += unit;
  }
  
  if (this->is_leaf()) {
    cout << tab << "ID: " << id_;
    return;
  }
  
  cout << tab << "Q(" << bytenum_ << ")->[";
  
  if (!this->is_leaf()) {
    offspring::const_iterator children_end = children_->end();
    for(offspring::const_iterator c_iter = children_->begin(); c_iter != children_end; c_iter++) {
    
      cout << tab + unit << ((unsigned) c_iter->first) << "->";
      c_iter->second->print_helper(spaces + 2);
      cout << " ";
    }
  }

  cout << tab << "]";

  // TODO
}

void lazy_trie::print() {
  print_helper(0);
}
