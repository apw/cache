#include "../includes/rep.h"
#include "../includes/d_trie.h"

#include "assert.h"
#include <iostream>

d_trie::d_trie(unsigned bytenum, unsigned id) {
  bytenum_ = bytenum;
  id_ = id;
  x_ = NULL;
}

d_trie::~d_trie(void) {
  if (x_exists()) {
    delete x_;
  }
  
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
  assert(0); // DEPRECATED
  
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

bool d_trie::x_exists() {
  return x_ != NULL;
}

void d_trie::extend_x(unsigned bytenum, unsigned id) {
  if (!x_exists()) {
    x_ = new d_trie(bytenum, id);
  }
}

d_trie *d_trie::decide_x() {
  return x_;
}

void d_trie::print_helper(unsigned spaces) {
  string tab("\n");
  string unit("||");
  for(unsigned i = 0; i < spaces; i++) {
    tab += unit;
  }
  
  if (this->is_leaf() && !this->x_exists()) {
    cout << tab << "ID: " << id_;
    return;
  }
  
  cout << tab << "Q(" << bytenum_ << ")->[";
  
  if (!this->is_leaf()) {
    offspring::const_iterator children_end = children_.end();
    for(offspring::const_iterator c_iter = children_.begin(); c_iter != children_end; c_iter++) {
    
      cout << tab + unit << ((unsigned) c_iter->first) << "->";
      c_iter->second->print_helper(spaces + 2);
      cout << " ";
    }
  }

  if (this->x_exists()) {
    cout << tab + unit << "X->";
    (this->decide_x())->print_helper(spaces + 2);
    cout << " ";
  }

  cout << tab << "]";
}

void d_trie::print() {
  print_helper(0);
}

void d_trie::graph_to_ofstream(ofstream& outfile, unsigned int cur_id,
			      unsigned int *nid) {
  if (this->is_leaf() && !this->x_exists()) {
    outfile << cur_id << " [label=\"" << get_id() << "\",shape=box]" << endl;
    return;
  }
  
  outfile << cur_id << " [label=\"" << get_bytenum() << "\"]" << endl;
  unsigned int next_id;
  if (!this->is_leaf()) {
    offspring::const_iterator children_end = children_.end();
    for(offspring::const_iterator c_iter = children_.begin(); c_iter != children_end; c_iter++) {
      
      next_id = (*nid)++;
      outfile << cur_id << " -> " << next_id << " [label=\"" << ((unsigned) c_iter->first) << "\"]" << endl;
      c_iter->second->graph_to_ofstream(outfile, next_id, nid);
    }
  }
  
  if (this->x_exists()) {
    // assert(!this->is_leaf()); // !!! this assert fails; how can a leaf have an x_path?!?!
    next_id = (*nid)++;
    outfile << cur_id << " -> " << next_id << " [label=\"X\"]" << endl;
    (this->decide_x())->graph_to_ofstream(outfile, next_id, nid);
  }
}

void d_trie::gen_graph(char *out_file_path) {
  assert(out_file_path);
  
  unsigned int node_id = 1;

  ofstream outfile;
  outfile.open(out_file_path);
  
  outfile << "digraph {" << endl;

  graph_to_ofstream(outfile, 0, &node_id);
  
  outfile << "}";

  outfile.close();
}

