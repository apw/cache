typedef struct intCollection {
  int *col; // collection of integers
  int size; // number of integers in col
  int index; // integer to "getInt" from
  int realMax; // actuall largest element in col
  int givenMax; // user-specified max
} intCollection;

/*
 * get an empty int collection to hold num int's in [0, max)
 */
intCollection *getEmptyIntCollection(int max, int num);

/*
 * generate num unique integers in [0, max)
 * returns a collection of these numbers on success, NULL
 * on failure
 */
intCollection *genUniqueInts(int max, int num);

/*
 * generates globally unique ints (across different intCollections)
 * across calls to this function (i.e. all values in intCollections
 * generated by this function will be unique, but no guarantees are
 * made about uniqueness of values in intCollections created using
 * other functions in conjunction with this one)
 */
intCollection *genGloballyUniqueInts(int max, int num);

/*
 * generate num "completely" random integers between (including) 0 and max
 * i.e. duplicates are allowed
 * there are scare-quotes around "completely" because I make no claims
 * about the distribution of variables being completely random, but
 * for most practical purposes it should be close enough to random
 */
intCollection *genRandomInts(int max, int num);

/*
 * copy an intCollection from src to dest; both must be pre-allocated
 * with one of the above functions that returns an intCollection *
 */
void copyIntCollection(intCollection *src, intCollection *dest);

/*
 * returns an int from col that we haven't "getInt-ed" already iff
 * such an int exists; If no such int exists, returns -1
 */
int getInt(intCollection *col);

// returns > 0 iff we have "getInt-ed" all ints in col
int gotAllInts(intCollection *col);

/*
 * resets the elements of intCollection so that they are all marked
 * as "never-been-getInt-ed"; THE INTEGERS THEMSELVES DON'T GET RESET,
 * AND NO MEMORY IS FREED; resetCollection exists to allow us to getInt
 * all the elements of col without having to reconstruct col from scratch
 */
void resetCollection(intCollection *col);

/*
 * returns 0 if "in" is not in "col". returns non-0 if "in" is in "col"
 */
int containsInt(int in, intCollection *col);

/*
 * clean up an int collection and release all of its memory
 */
void destroyIntCollection(intCollection *col);
