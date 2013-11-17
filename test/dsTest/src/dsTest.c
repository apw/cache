#include <iterator>

#include <stdio.h>
#include <assert.h>
#include "../include/intGen.h"
#include "../../../includes/rset_uint.h"

#define TESTMAX (1 << 16)
#define TESTNUM (1 << 10)
#define RESETNUM (1 << 3)
#define SKIPSIZE 5

//#define TESTMAX 10
//#define TESTNUM 9
//#define RESETNUM 8

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

void test_simple(void) {
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
    assert(col != NULL);

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
  return;
}

int main(void) {
  test_simple();
  return 0;
}
