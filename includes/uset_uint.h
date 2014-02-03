#ifndef _USET_UINT_H
#define _USET_UINT_H

#include <vector>

using namespace std;

class uset_uint {
 public:
  uset_uint(unsigned);
  ~uset_uint(void);

  void begin_trans(void);
  void end_trans(void);
  void undo_trans(void);
  bool lookup(unsigned);
  bool remove(unsigned);

  class iterator {
  public:
    iterator(uset_uint *);
    ~iterator(void);

    bool is_cur_valid(void);
    unsigned get_cur(void);
    void next(void);
    void remove_cur(void);

  private:
    unsigned cur_element_;
    uset_uint *uset_;
  };

  iterator get_iterator(void);

  unsigned get_size(void);
  unsigned get_capacity(void);

  void print(void);
  
 private:
  unsigned capacity_;
  unsigned size_;

  unsigned *set_;

  typedef vector<unsigned> remove_set;
  typedef vector<remove_set *> undo_set;
  
  undo_set u_;

  bool in_trans_;
};

#endif
