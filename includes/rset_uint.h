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
    iterator(unsigned);
    ~iterator(void);

    bool has_next(void);
    bool get_cur(void);
    void next(void);

  private:
    unsigned assigned_vnum_;
    unsigned cur_node_;
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

  int get_next(unsigned);
  int get_prev(unsigned);
};

#endif
