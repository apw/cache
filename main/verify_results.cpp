#include <iostream>
#include <fstream>
#include <assert.h>

#include "../includes/verify_results.h"

using namespace std;

void verify_results(int num_files, char **files) {
  int i, j, cur_char;
  std::string cur_string;

  // holds ifstreams for files
  std::ifstream fileArray[num_files];

  // open ifstreams
  for (i = 0; i < num_files; i++) {
    fileArray[i].open(files[i], std::ifstream::in);
  }

  // iterate over lines in files (assumes there are same number of lines in all
  // files)
  while (1) {
    getline(fileArray[0], cur_string);
    
    // if we hit end of first file
    if(fileArray[0].eof()) {
      // assert we are at the end of all files
      for (j = 1; j < num_files; j++) {
	getline(fileArray[j], cur_string);
	assert(fileArray[j].eof());
      }
      
      // we are done
      return;
    }

    // otherwise, we need to compare the chars at the beginnings of
    // the strings to see if they are the same ('M' vs 'H')

    // get first char at this line in the first file
    cur_char = cur_string.at(0);

    // assert it is the same as the first character of that line in all other
    // files
    for (j = 1; j < num_files; j++) {
      getline(fileArray[j], cur_string);
      assert(cur_char == cur_string.at(0));
    }
  }
}
