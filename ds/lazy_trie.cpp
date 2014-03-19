#include "../includes/rep.h"
#include "../includes/lazy_trie.h"

#include "assert.h"
#include <iostream>

lazy_trie::lazy_trie(unsigned id, unsigned cur_index,
		     unsigned num_relevant, unsigned *relevant) {
  assert(cur_index <= num_relevant);
  
  id_ = id;  
  cur_index_ = cur_index;
  num_relevant_ = num_relevant;
  relevant_ = relevant;
  
  if (cur_index_ == num_relevant) {
    bytenum_ = INVALID_BYTENUM;
  } else {
    bytenum_ = relevant[cur_index];
  }
  
  ee_ = NULL;
  
  // node starts its life cycle lazy
  ls_ = new lazy_store();
  children_ = NULL;
}

lazy_trie::~lazy_trie(void) {
  if (ls_ != NULL) {
    delete ls_;
  }
  
  if (children_ != NULL) {
    offspring::const_iterator children_end = children_->end();
    for(offspring::const_iterator c_iter = children_->begin(); c_iter != children_end; c_iter++) {
      delete c_iter->second;
    }
    
    delete children_;
  }
  
  if (ee_ != NULL) {
    delete ee_;
  }
}

bool lazy_trie::is_lazy() {
  assert((ls_ == NULL && children_ != NULL) || (ls_ != NULL && children_ == NULL));
  return children_ == NULL;
}

bool lazy_trie::ee_exists() {
  return ee_ != NULL;
}

unsigned lazy_trie::burst() {
  assert(is_lazy());
  
  assert(cur_index_ + 1 <= num_relevant_);
  
  unsigned steps = 0;
  
  tr1::unordered_set<unsigned> x_set;
  children_ = new offspring();
  // iterate over things to insert
  for(unsigned i = 0; i < ls_->size(); i++) {
    bool x_needed = true;
    
    steps++;
    
    // stupidly find appropriate placement in each vector
    for(unsigned j = 0; j < ls_->operator[](i).ve.size(); j++) {
      // insert rest of vector into children
      if (ls_->operator[](i).ve[j].bytenum == relevant_[cur_index_]) {
	uint8_t byteval = ls_->operator[](i).ve[j].byteval;
	unsigned id = ls_->operator[](i).id;
	
	if (cur_index_ + 1 == num_relevant_) {
	  // this node is a leaf and thus the next one should just have an ID
	  
	  if (children_->count(byteval) == 0) {
	    lazy_trie *child = new lazy_trie(id, cur_index_ + 1,
					     num_relevant_, relevant_);
	    children_->operator[](byteval) = child;
	  }
	  
	  children_->operator[](byteval)->add_vect(ls_->operator[](i));	  
	} else {
	  // this node is not a leaf
	  
	  if (children_->count(byteval) == 0) {
	    lazy_trie *child = new lazy_trie(INVALID_ID, cur_index_ + 1,
					     num_relevant_, relevant_);
	    children_->operator[](byteval) = child;
	  }
	  
	  children_->operator[](byteval)->add_vect(ls_->operator[](i));
	}
	
	x_needed = false;
	break;
      }
    }
    
    // vector has current bytenum as don't care - remember its id
    // for later exp insertion
    if (x_needed) {
      x_set.insert(i);
    }
  }
  
  if (x_set.size() > 0) {
    // create new ee path
    assert(!ee_exists());
    if (cur_index_ + 1 == num_relevant_) {
      ee_ = new lazy_trie(ls_->operator[](0).id, cur_index_ + 1,
			  num_relevant_, relevant_);
    } else {
      ee_ = new lazy_trie(INVALID_ID, cur_index_ + 1, num_relevant_, relevant_);
    }
    
    // iterate over vectors that have current bytenum as don't care
    tr1::unordered_set<unsigned>::const_iterator iter_end = x_set.end();
    for (tr1::unordered_set<unsigned>::const_iterator iter = x_set.begin();
	 iter != iter_end; iter++) {
      steps++;
      
      // insert vector into ee path
      ee_->add_vect(ls_->operator[](*iter));
    
      // insert vector into all existing child paths
      offspring::const_iterator children_end = children_->end();
      for(offspring::const_iterator c_iter = children_->begin(); c_iter != children_end; c_iter++) {
	c_iter->second->add_vect(ls_->operator[](*iter));
      }
    }
  }
  
  delete ls_;
  ls_ = NULL;

  return steps;
}

lazy_trie *lazy_trie::decide(uint8_t byteval, unsigned *steps) {
  assert(*steps == 0);
  
  if (this->is_lazy()) {
    *steps += this->burst();
  }
  
  *steps += 1;
  
  if (children_->count(byteval) == 0) {
    if (ee_exists()) {
      return ee_;
    }
    
    return NULL;
  }
  
  return children_->operator[](byteval);
}

void lazy_trie::add_vect(c_entry ce) {
  if (this->is_lazy()) {
    ls_->push_back(ce);
    return;
  }
  
  // Operation not yet supported: adding vect to burst-node
  assert(0);
  
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
  if (is_lazy()) {
    return cur_index_ == num_relevant_;
  }
  
  return !ee_exists() && children_->empty();
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
  
  if (this->is_leaf() && !this->is_lazy()) {
    cout << tab << "ID: " << id_;
    return;
  }
  
  if (this->is_lazy()) {
    cout << tab << "Z(" << bytenum_ << ")->[";
    
    for(unsigned i = 0; i < ls_->size(); i++) {
      cout << ls_->operator[](i).id;
      if (i < ls_->size() - 1) {
	cout << ", ";
      }
    }
    
    cout << "]";
  } else {
    cout << tab << "Q(" << bytenum_ << ")->[";
    
    if (!this->is_leaf()) {
      offspring::const_iterator children_end = children_->end();
      for(offspring::const_iterator c_iter = children_->begin(); c_iter != children_end; c_iter++) {
	
	cout << tab + unit << ((unsigned) c_iter->first) << "->";
	c_iter->second->print_helper(spaces + 2);
	cout << " ";
      }
    }

    if (this->ee_exists()) {
	cout << tab + unit << "EE->";
	ee_->print_helper(spaces + 2);
	cout << " ";      
    }
    
    cout << tab << "]";
  }
}

void lazy_trie::print() {
  assert(0); // TODO DOES NOT YET WORK
  print_helper(0);
}

void lazy_trie::graph_to_ofstream(ofstream& outfile, unsigned int cur_id,
			      unsigned int *nid) {
  if (this->is_leaf() && !this->is_lazy()) {
    outfile << cur_id << " [label=\"" << get_id() << "\",shape=box]" << endl;
    return;
  }

  if (this->is_lazy()) {
    outfile << cur_id << " [label=\"";

    for(unsigned i = 0; i < ls_->size(); i++) {
      outfile << ls_->operator[](i).id;
      if (i < ls_->size() - 1) {
	outfile << ", ";
      }
    }
    
    outfile << "\",shape=box]" << endl;
    return;
  }
  
  outfile << cur_id << " [label=\"" << get_bytenum() << "\"]" << endl;
  unsigned int next_id;
  if (!this->is_leaf()) {
    offspring::const_iterator children_end = children_->end();
    for(offspring::const_iterator c_iter = children_->begin(); c_iter != children_end; c_iter++) {

      next_id = (*nid)++;
      outfile << cur_id << " -> " << next_id << " [label=\"" << ((unsigned) c_iter->first) << "\"]" << endl;
      c_iter->second->graph_to_ofstream(outfile, next_id, nid);
    }
  }
  
  if (this->ee_exists()) {
    // assert(!this->is_leaf()); // !!! this assert fails; how can a leaf have an x_path?!?!
    next_id = (*nid)++;
    outfile << cur_id << " -> " << next_id << " [label=\"X\"]" << endl;
    ee_->graph_to_ofstream(outfile, next_id, nid);
  }
}

void lazy_trie::gen_graph(char *out_file_path) {
  assert(out_file_path);
  
  unsigned int node_id = 1;

  ofstream outfile;
  outfile.open(out_file_path);
  
  outfile << "digraph {" << endl;

  graph_to_ofstream(outfile, 0, &node_id);
  
  outfile << "}";

  outfile.close();
}
