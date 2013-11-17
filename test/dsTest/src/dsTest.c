#include <iterator>

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "../include/intGen.h"
#include "../../../includes/rset_uint.h"
#include "../../../includes/uset_uint.h"

/*
 * TESTMAX should be significantly larger than (TESTNUM * MAXDO);
 * if this is not the case, test_undo_n_transactions will enter
 * an infinite-loop trying to generate unique numbers because
 * genUniqueInts doesn't to error checking for this scenario yet
 */
#define TESTMAX (1 << 20)
#define TESTNUM (1 << 10)
#define RESETNUM (1 << 3)
#define MAXDO (1 << 4)

void standard_default(void) {
  // get unique collection
  intCollection *col = genUniqueInts(TESTMAX, TESTNUM);

  printf("SECTION I: ALL NUMBERS SHOULD BE UNIQUE\n");
  
  // print out collection to see if results makes sense
  for (int i = 0; i < TESTNUM; i++) {
    // test invariants on gotAllInts
    assert(!gotAllInts(col));
    printf("%d ", getInt(col));
  }
  printf("\n");

  // test invariants on gotAllInts
  assert(gotAllInts(col));

  // test resetCollection
  resetCollection(col);

  // make output easier to parse by human eyes
  printf("--------------\n");
  printf("SECTION II: ALL NUMBERS SHOULD BE THE ");
  printf("SAME AS IN SECTION I\n");

  // should get same collection as before resetCollection(col)
  for (int i = 0; i < TESTNUM; i++) {
    // test invariants on gotAllInts
    assert(!gotAllInts(col));
    printf("%d ", getInt(col));
  }
  printf("\n");

  // test invariants on gotAllInts
  assert(gotAllInts(col));

  // make output easier to parse by human eyes
  printf("--------------\n");
  printf("SECION III: REPEATED NUMBERS ARE ALLOWED ");
  printf("BUT NOT GUARANTEED\n");

  // get random collection
  intCollection *rCol = genRandomInts(TESTMAX, TESTNUM);

  for (int i = 0; i < TESTNUM; i++) {
    // test invariants on gotAllInts
    assert(!gotAllInts(rCol));
    printf("%d ", getInt(rCol));
  }
  printf("\n");

  // test invariants on gotAllInts
  assert(gotAllInts(rCol));

  // clean up and return
  destroyIntCollection(col);
}

void rset_uint_test_simple(void) {
  printf("[BEGINNING] rset_uint_test_simple\n");
  // get unique int collection
  intCollection *col;

  // get the rset_uint to test
  rset_uint *r = new rset_uint(TESTMAX);
  assert(r);

  // do RESETNUM iterations
  for (int counter = 0; counter < RESETNUM; counter++) {
    printf("[%d]\r", counter);
    fflush(stdout);
    // get unique ints to remove from the rset
    col = genUniqueInts(TESTMAX, TESTNUM);
    assert(col);

    // remove some random elements
    unsigned i;
    int cur_int;
    bool success;
    for (i = 0; i < TESTNUM; i++) {
      cur_int = getInt(col);
      success = r->remove(cur_int);
      assert(success);
    }
    
    // check that the removed elements are removed and the others are still there
    for(i = 0; i < TESTNUM; i++) {
      success = r->lookup(i);
      if (containsInt(i, col)) {
	assert(!success);
      } else {
	assert(success);
      }
    }
    
    // check that the iterator works properly
    rset_uint::iterator it = r->get_iterator();
    
    // check that all things the iterator returns are not ones we removed
    int iter_count = 0;
    while(it.is_cur_valid()) {
      cur_int = it.get_cur();
      assert(!containsInt(cur_int, col));
      iter_count++;
      it.next();
    }
    
    // assert the iterator iterated over the right number of entries
    assert(iter_count == (TESTMAX - TESTNUM));

    r->restore();
    destroyIntCollection(col);
  }

  // clean up and return
  delete r;
  printf("[PASSED] rset_uint_test_simple\n");
  return;
}

static void undo_n_transactions(uset_uint *us, unsigned n) {
  for (int i = 0; i < n; i++) {
    us->undo_trans();
  }
}

static bool remove_all_col_vals_from_uset(intCollection *col, uset_uint *us) {
  int cur_int;
  bool res;
  while (!gotAllInts(col)) {
    cur_int = getInt(col);
    res = us->remove((unsigned) cur_int);
    if (res == false) {
      return false;
    }
  }

  return true;
}

static bool col_vals_uset_helper(intCollection *col, uset_uint *us, bool b) {
  int cur_int;
  bool res;
  while (!gotAllInts(col)) {
    cur_int = getInt(col);
    res = us->lookup((unsigned) cur_int);
    if (res == b) {
      return false;
    }
  }

  return true;  
}

static bool all_col_vals_in_uset(intCollection *col, uset_uint *us) {
  return col_vals_uset_helper(col, us, true);
}

static bool all_col_vals_not_in_uset(intCollection *col, uset_uint *us) {
  return col_vals_uset_helper(col, us, false);
}

static void test_do_n_undo_m_transactions(int n, int m) {
  // can't undo more transactions than we do
  assert(m <= n);

  // get a uset_uint to test
  uset_uint *us = new uset_uint(TESTMAX);

  // get n intCollections, each with values unique across all collections
  int i;
  intCollection *cols[n];
  bool res;
  for (i = 0; i < n; i++) {
    cols[i] = genGloballyUniqueInts(TESTMAX, TESTNUM);
    assert(cols[i] != NULL);
  }

  // remove the values in each of the n intCollections in turn
  for (i = 0; i < n; i++) {
    us->begin_trans();
    res = remove_all_col_vals_from_uset(cols[i], us);
    assert(res == true);
    us->end_trans();
  }

  // undo last m transactions
  for (i = 0; i < m; i++) {
    us->undo_trans();
  }

  /*
   * verify that the transactions that weren't undone still have their values
   * removed
   */
  int num_valid_transactions = n-m;
  int last_valid_transaction_index = num_valid_transactions-1;
  for (i = 0; i < num_valid_transactions; i++) {
    res = all_col_vals_not_in_uset(cols[i], us);
  }

   // verify that the transactions that were undone have their values present
  for(; i < n; i++) {
    res = all_col_vals_in_uset(cols[i], us);
  }

  // clean up and return
  for (i = 0; i < n; i++) {
    destroyIntCollection(cols[i]);
  }

  delete us;
  return;
}

void uset_uint_test_simple(void) {
  printf("[BEGINNING] uset_uint_test_simple\n");
  for (int i = 0; i < RESETNUM; i++) {
    printf("[%d]\r", i);
    
    // test that we can do and undo 0 transactions
    test_do_n_undo_m_transactions(0, 0);

    // test that we can do and undo 1 transaction
    test_do_n_undo_m_transactions(1, 1);

    // test that we can do and undo the same number of transactions
    srand(time(NULL));
    int num_do = rand() % MAXDO;
    test_do_n_undo_m_transactions(num_do, num_do);
    
    // test that we can do and undo a different number of transactions
    int num_undo;
    if (num_do == 0) {
      num_undo = 0;
    } else {
      num_undo = rand() % MAXDO;
    }
    test_do_n_undo_m_transactions(num_do, num_undo);
    
  }

  printf("[PASSED] uset_uint_test_simple\n");
  return;
}

void uset_uint_test_errors(void) {
  // test that if we try to undo more times than redo, something logical happens
  printf("[BEGINNING] uset_uint_test_errors\n");
  assert(0);
  printf("[PASSED] uset_uint_test_errors\n");
}

int main(void) {
  //(*** TODO TEST SIZE OF USET_UINT ***)
  //  rset_uint_test_simple();
  uset_uint_test_simple();
  uset_uint_test_errors();
  return 0;
}
