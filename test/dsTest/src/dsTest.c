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
#define TESTMAX (1 << 16)
#define TESTNUM (1 << 8)
#define RESETNUM (1 << 3)
#define MAXDO (1 << 4)
#define NUMREMOVE (1 << 6)

void standard_default(void) {
  // get unique collection
  intCollection *col = genUniqueInts(TESTMAX, TESTNUM);

  printf("SECTION I: ALL NUMBERS SHOULD BE UNIQUE\n");
  fflush(stdout);
  
  // print out collection to see if results makes sense
  for (int i = 0; i < TESTNUM; i++) {
    // test invariants on gotAllInts
    assert(!gotAllInts(col));
    printf("%d ", getInt(col));
    fflush(stdout);
  }
  printf("\n");
  fflush(stdout);

  // test invariants on gotAllInts
  assert(gotAllInts(col));

  // test resetCollection
  resetCollection(col);

  // make output easier to parse by human eyes
  printf("--------------\n");
  printf("SECTION II: ALL NUMBERS SHOULD BE THE ");
  printf("SAME AS IN SECTION I\n");
  fflush(stdout);

  // should get same collection as before resetCollection(col)
  for (int i = 0; i < TESTNUM; i++) {
    // test invariants on gotAllInts
    assert(!gotAllInts(col));
    printf("%d ", getInt(col));
    fflush(stdout);
  }
  printf("\n");
  fflush(stdout);

  // test invariants on gotAllInts
  assert(gotAllInts(col));

  // make output easier to parse by human eyes
  printf("--------------\n");
  printf("SECION III: REPEATED NUMBERS ARE ALLOWED ");
  printf("BUT NOT GUARANTEED\n");
  fflush(stdout);

  // get random collection
  intCollection *rCol = genRandomInts(TESTMAX, TESTNUM);

  for (int i = 0; i < TESTNUM; i++) {
    // test invariants on gotAllInts
    assert(!gotAllInts(rCol));
    printf("%d ", getInt(rCol));
    fflush(stdout);
  }
  printf("\n");
  fflush(stdout);

  // test invariants on gotAllInts
  assert(gotAllInts(rCol));

  // clean up and return
  destroyIntCollection(col);
}

void rset_uint_test_simple(void) {
  printf("[BEGINNING] rset_uint_test_simple\n");
  fflush(stdout);
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
  fflush(stdout);
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
  assert(col->index == 0);
  while (!gotAllInts(col)) {
    cur_int = getInt(col);
    res = us->remove((unsigned) cur_int);
    if (res == false) {
      resetCollection(col);
      return false;
    }
  }

  resetCollection(col);
  return true;
}

static bool col_vals_uset_helper(intCollection *col, uset_uint *us, bool b) {
  int cur_int;
  bool res;
  while (!gotAllInts(col)) {
    cur_int = getInt(col);
    res = us->lookup((unsigned) cur_int);
    if (res != b) {
      resetCollection(col);
      return false;
    }
  }

  resetCollection(col);
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
  assert(us);

  // get n intCollections, each with values unique across all collections
  int i;
  intCollection *cols[n];
  bool res;
  for (i = 0; i < n; i++) {
    cols[i] = genGloballyUniqueInts(TESTMAX, TESTNUM);
    assert(cols[i] != NULL);
  }

  assert(us->get_size() == TESTMAX);

  // remove the values in each of the n intCollections in turn
  for (i = 0; i < n; i++) {
    us->begin_trans();
    res = remove_all_col_vals_from_uset(cols[i], us);
    assert(res == true);
    us->end_trans();
    assert(us->get_size() == TESTMAX-((i+1)*TESTNUM));
  }

  assert(us->get_size() == TESTMAX-((n)*TESTNUM));

  // undo last m transactions
  for (i = 0; i < m; i++) {
    us->undo_trans();
    assert(us->get_size() == (TESTMAX-((n)*TESTNUM))+((i+1)*TESTNUM));
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

void uset_double_remove_diff_trans(void) {
  printf("[BEGINNING] uset_double_remove_diff_trans\n");
  intCollection *col = genUniqueInts(TESTMAX, TESTNUM);
  assert(col);

  uset_uint *us = new uset_uint(TESTMAX);
  assert(us);

  bool res = all_col_vals_in_uset(col, us);
  assert(res == true);

  us->begin_trans();
  res = remove_all_col_vals_from_uset(col, us);
  assert(res == true);
  us->end_trans();

  res = all_col_vals_not_in_uset(col, us);
  assert(res == true);

  us->begin_trans();
  res = remove_all_col_vals_from_uset(col, us);
  assert(res == false);
  us->end_trans();

  res = all_col_vals_not_in_uset(col, us);
  assert(res == true);

  us->undo_trans();
  
  res = all_col_vals_not_in_uset(col, us);
  assert(res == true);

  us->undo_trans();
  
  res = all_col_vals_in_uset(col, us);
  assert(res == true);
  
  // clean up and return
  destroyIntCollection(col);
  delete us;
  printf("[PASSED] uset_double_remove_diff_trans\n");
  return;
}

void uset_double_remove_same_trans(void) {
  printf("[BEGINNING] uset_double_remove_same_trans\n");
  intCollection *col = genUniqueInts(TESTMAX, TESTNUM);
  assert(col);

  uset_uint *us = new uset_uint(TESTMAX);
  assert(us);

  bool res = all_col_vals_in_uset(col, us);
  assert(res == true);

  us->begin_trans();
  res = remove_all_col_vals_from_uset(col, us);
  assert(res == true);

  res = all_col_vals_not_in_uset(col, us);
  assert(res == true);

  res = remove_all_col_vals_from_uset(col, us);
  assert(res == false);
  us->end_trans();

  res = all_col_vals_not_in_uset(col, us);
  assert(res == true);

  us->undo_trans();
  
  res = all_col_vals_in_uset(col, us);
  assert(res == true);

  // clean up and return
  destroyIntCollection(col);
  delete us;
  printf("[PASSED] uset_double_remove_same_trans\n");
  return;
}

void uset_uint_test_simple(void) {
  printf("[BEGINNING] uset_uint_test_simple\n");
  fflush(stdout);
  for (int i = 0; i < RESETNUM; i++) {
    printf("[%d]\r", i);
    fflush(stdout);
    
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
      num_undo = rand() % num_do;
    }
    test_do_n_undo_m_transactions(num_do, num_undo);
    
  }

  printf("[PASSED] uset_uint_test_simple\n");
  fflush(stdout);
  return;
}

void uset_uint_test_errors(void) {
  // test that if we try to undo more times than redo, something logical happens
  printf("[BEGINNING] uset_uint_test_errors\n");
  printf("We expect the following to FAIL AN ASSERT\n");
  fflush(stdout);

  uset_uint *us = new uset_uint(TESTMAX);
  assert(us);

  /*
   *test that uset_uint fails gracefully whn we try to undo withou doing a
   * transaction first
   */
  us->undo_trans();
  return;
}


static void uset_uint_one_iterator_simple_walk(int n, int m) {
  // can't undo more transactions than we do
  assert(m <= n);

  // get a uset_uint and associated iterator to test
  uset_uint *us = new uset_uint(TESTMAX);
  assert(us);
  uset_uint::iterator it = us->get_iterator();

  // get n intCollections, each with values unique across all collections
  int i;
  intCollection *cols[n];
  bool res;
  for (i = 0; i < n; i++) {
    cols[i] = genGloballyUniqueInts(TESTMAX, TESTNUM);
    assert(cols[i] != NULL);
  }

  assert(us->get_size() == TESTMAX);

  // remove the values in each of the n intCollections in turn
  for (i = 0; i < n; i++) {
    us->begin_trans();
    res = remove_all_col_vals_from_uset(cols[i], us);
    assert(res == true);
    us->end_trans();
    assert(us->get_size() == TESTMAX-((i+1)*TESTNUM));
  }

  assert(us->get_size() == TESTMAX-((n)*TESTNUM));

  // undo last m transactions
  for (i = 0; i < m; i++) {
    us->undo_trans();
    assert(us->get_size() == (TESTMAX-((n)*TESTNUM))+((i+1)*TESTNUM));
  }

  // verify that the iterator returns only values we didn't remove
  int cur_int;
  int num_valid_transactions = n-m;
  int num_seen = 0;
  while (it.is_cur_valid()) {
    printf("[%d]\r", num_seen);
    fflush(stdout);

    num_seen++;
    cur_int  = (int) it.get_cur();

    // make sure cur_int is in the right range
    assert(cur_int < TESTMAX && cur_int >= 0);

    // make sure cur_int is not an int we removed from us
    if (n > 0) {
      for (i = 0; i < num_valid_transactions; i++) {
	assert(!containsInt(cur_int, cols[i]));
	}
      }

    it.next();
  }

  // make sure we iterated over the expected number of ints
  assert(num_seen == TESTMAX-(num_valid_transactions*TESTNUM));

  /*
   * verify that the transactions that weren't undone still have their values
   * removed
   */
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


static void uset_uint_iterator_remove_cur(void) {
  // get a uset_uint and associated iterator to test
  uset_uint *us = new uset_uint(TESTMAX);
  assert(us);
  uset_uint::iterator it = us->get_iterator();
  int removed[NUMREMOVE];
  int num_removed = 0;
  int cur_int;
  int num_iter = 0;

  srand(time(NULL));
  while (it.is_cur_valid()) {
    printf("1: [%d]\r", num_iter);
    num_iter++;

    us->begin_trans();
    cur_int = (int) it.get_cur();

    // remove_cur an element with probability 0.5 and remember it if removed
    if (num_removed < NUMREMOVE && rand() % 2 == 0) {
      it.remove_cur();
      removed[num_removed] = cur_int;
      num_removed++;
      us->end_trans();
      continue;
    }

    us->end_trans();
    it.next();
  }
  
  for (int i = 0; i <= num_removed; i++) {
    printf("2: [%d]\r", i);
    uset_uint::iterator iter = us->get_iterator();
    int num_valid = 0;

    while (iter.is_cur_valid()) {
      num_valid++;
      cur_int = iter.get_cur();
      
      // sanity check
      assert(cur_int < TESTMAX);

      // make sure cur_int is not one of the removed ints
      for (int j = num_removed-i; j < num_removed; j++) {
	assert(cur_int != removed[j]);
      }
      
      iter.next();
    }

    // make sure we see the right number of ints
    assert(num_valid == i+TESTMAX-num_removed);

    // don't want to undo after the last check
    if (i != num_removed) {
      us->undo_trans();
    }
  }

  delete us;
  return;
}


void uset_uint_test_iterator(void) {
  printf("[BEGINNING] uset_uint_test_iterator\n");
  uset_uint_iterator_remove_cur();

  uset_uint_one_iterator_simple_walk(0, 0);
  uset_uint_one_iterator_simple_walk(1, 1);

  srand(time(NULL));
  int num_do = rand() % MAXDO;
  uset_uint_one_iterator_simple_walk(num_do, num_do);

  int num_undo;
  if (num_do > 0) {
    num_undo = rand() % num_do;
  } else {
    num_undo = 0;
  }
  uset_uint_one_iterator_simple_walk(num_do, num_undo);

  
  printf("[PASSED] uset_uint_test_iterator\n");
  return;
}

int main(void) {
  //  (*** TODO TEST ITERATOR ***)
  //uset_uint_test_simple();
  //uset_double_remove_diff_trans();
  //uset_double_remove_same_trans();
  uset_uint_test_iterator();
  //uset_uint_test_errors();
  //rset_uint_test_simple();
  return 0;
}
