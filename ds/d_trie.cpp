#include "../includes/rep.h"
#include "../includes/d_trie.h"

#include "assert.h"
#include <iostream>

d_trie::d_trie(unsigned bytenum, unsigned id) {
  bytenum_ = bytenum;
  id_ = id;
}

d_trie::~d_trie(void) {
  offspring::const_iterator children_end = children_.end();
  for(offspring::const_iterator c_iter = children_.begin(); c_iter != children_end; c_iter++) {
    delete c_iter->second;
  }
}

d_trie *d_trie::decide(uint8_t byteval) {
  if (children_.count(byteval) == 0) {
    return NULL;
  }
  
  return children_[byteval];
}

void d_trie::extend(uint8_t byteval, unsigned bytenum, unsigned id) {
  if (children_.count(byteval) == 0) {
    children_[byteval] = new d_trie(bytenum, id);
  } else {
    assert(children_[byteval]->get_bytenum() == bytenum);
  }
}

void d_trie::add_vect(uint8_t *bv, unsigned len, unsigned id) {
  d_trie *current = this;
  for(unsigned i = 0; i < len; i++) {
    if (current->children_.count(bv[i]) > 0) {
      current = current->children_[bv[i]];
    } else {
      d_trie *d = new d_trie(i, id);
      current->children_[bv[i]] = d;
      current = d;
    }
  }
}

bool d_trie::is_leaf() {
  return children_.empty();
}

unsigned d_trie::get_bytenum() {
    return bytenum_;
}

unsigned d_trie::get_id() {
  return id_;
}

void d_trie::print(void) {
  cout << "[ ";
  offspring::const_iterator children_end = children_.end();
  for(offspring::const_iterator c_iter = children_.begin(); c_iter != children_end; c_iter++) {
    cout << ((unsigned) c_iter->first) << "->";
    c_iter->second->print();
    cout << " ";
  }

  cout << "]";
}
