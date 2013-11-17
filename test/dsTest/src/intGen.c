#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include "../include/intGen.h"


static intCollection *globalIntCollection = NULL;

static intCollection *getEmptyIntCollection(int max, int num);

static void uniqueHelper(intCollection *srcCol,
			 intCollection *destCol, int max, int num) {
  /*
   * currently just keep trying until get unique numbers;
   * should do something smarter
   */
  int curRand;
  int numUnique = 0;
  srand(time(NULL)); // seed rand for each call to genUniqueInts
  while(numUnique < num) {
    // get random numbers until we get a unique number
    curRand = rand() % max;
    if (containsInt(curRand, srcCol)) {
      continue;
    }

    // store this number and update metadata in dest
    destCol->col[numUnique] = curRand;
    if (curRand > destCol->realMax) {
      destCol->realMax = curRand;
    }

    // if src is not the same as dest, store the same data in dest
    if (srcCol != destCol) {
      srcCol->col[numUnique] = curRand;
      if (curRand > srcCol->realMax) {
	srcCol->realMax = curRand;
      }
    }

    numUnique++;
  }  
}

intCollection *genUniqueInts(int max, int num) {
  if (max > RAND_MAX) {
    return NULL;
  }

  intCollection *intCol = getEmptyIntCollection(max, num);
  if (intCol == NULL) {
    return NULL;
  }

  // actually generate the unique ints
  uniqueHelper(intCol, intCol, max, num);

  return intCol;
}

static void copyIntCollection(intCollection *src, intCollection *dest) {
  assert(src);
  assert(dest);

  dest->size = src->size;
  dest->index = src->index;
  dest->realMax = src->realMax;
  dest->givenMax = src->givenMax;

  for (int i = 0; i < src->size; i++) {
    dest->col[i] = src->col[i];
  }

  return;
}

intCollection *genGloballyUniqueInts(int max, int num) {
  intCollection *res = getEmptyIntCollection(max, num);
  assert(res);

  if (globalIntCollection == NULL) {
    globalIntCollection = genUniqueInts(max, num);
    copyIntCollection(globalIntCollection, res);
  } else {
    uniqueHelper(globalIntCollection, res, max, num);
  }

  return res;
}

intCollection *genRandomInts(int max, int num) {
  if (max > RAND_MAX) {
    return NULL;
  }

  intCollection *intCol = getEmptyIntCollection(max, num);
  if (intCol == NULL) {
    return NULL;
  }

  int curRand;
  srand(time(NULL)); // seed rand for each call to genRandomInts
  for (int i = 0; i < num; i++) {
    curRand = rand() % max;

    // store this number and update metadata
    intCol->col[i] = curRand;
    if (curRand > intCol->realMax) {
      intCol->realMax = curRand;
    }
  }

  return intCol;
}


int getInt(intCollection *col) {
  return col->col[(col->index)++];
}

int gotAllInts(intCollection *col) {
  return (col->index >= col->size);
}

void resetCollection(intCollection *col) {
  col->index = 0;
  return;
}

void destroyIntCollection(intCollection *col) {
  free(col->col);
  free(col);
  return;
}

static intCollection *getEmptyIntCollection(int max, int num) {
  intCollection *ret = (intCollection *) malloc(sizeof(intCollection));
  if (ret == NULL) {
    return NULL;
  }
  
  int *col = (int *) malloc(sizeof(int) * num);
  if (col == NULL) {
    free(ret);
    return NULL;
  }

  // probably unnecessary, but I'm paranoid
  for (int i = 0; i < num; i++) {
    col[i] = -1;
  }

  ret->col = col;
  ret->size = num;
  ret->index = 0;
  ret->realMax = -1;
  ret->givenMax = max;
  
  return ret;
}

int containsInt(int in, intCollection *col) {
  for (int i = 0; i < col->size; i++) {
    if (col->col[i] == in) {
      return 1;
    }
  }

  return 0;
}
