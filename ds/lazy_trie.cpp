#include "../includes/rep.h"
#include "../includes/lazy_trie.h"

#include "assert.h"
#include <iostream>

lazy_trie::lazy_trie(unsigned bytenum, unsigned id) {
  bytenum_ = bytenum;
  id_ = id;
}

lazy_trie::~lazy_trie(void) {
  offspring::const_iterator children_end = children_->end();
  for(offspring::const_iterator c_iter = children_->begin(); c_iter != children_end; c_iter++) {
    delete c_iter->second;
  }
}

lazy_trie *lazy_trie::decide(uint8_t byteval) {
  if (children_->count(byteval) == 0) {
    return NULL;
  }
  
  return children_->operator[](byteval);
}

void lazy_trie::extend(uint8_t byteval, unsigned bytenum, unsigned id) {
  if (children_->count(byteval) == 0) {
    children_->operator[](byteval) = new lazy_trie(bytenum, id);
  } else {
    assert(children_->operator[](byteval)->get_bytenum() == bytenum);
  }
}

void lazy_trie::add_vect(vect *bv, unsigned id) {
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
}

bool lazy_trie::is_leaf() {
  return children_->empty();
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
}

void lazy_trie::print() {
  print_helper(0);
}
