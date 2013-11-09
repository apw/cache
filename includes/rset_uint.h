#ifndef _RSET_UINT_H
#define _RSET_UINT_H

using namespace std;

class rset_uint {
 public:
  rset_uint(unsigned);
  ~rset_uint(void);

  void restore(void);
  bool lookup(unsigned);
  bool remove(unsigned);

  class iterator {
  public:
    iterator(rset_uint *);
    ~iterator(void);

    bool is_cur_valid(void);
    unsigned get_cur(void);
    void next(void);

  private:
    unsigned assigned_vnum_;
    unsigned cur_node_;
    rset_uint *rset_;
  };

  iterator get_iterator(void);

 private:
  unsigned cur_vnum_;
  unsigned size_;

  typedef struct node {
    unsigned is_present;
    unsigned prev;
    unsigned next;
    unsigned n_vnum;
    unsigned p_vnum;
  } node;

  node *set_;
  unsigned first_;

  unsigned get_next(unsigned);
  unsigned get_prev(unsigned);

  bool is_valid(unsigned);
};

#endif
