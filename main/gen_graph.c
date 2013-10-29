#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define COMMAND_LEN 100
#define MAX_FNAME_LEN 50

int gen_graph(const char *data_file) {
  assert(strlen(data_file) < MAX_FNAME_LEN);
  char command[COMMAND_LEN];
  sprintf(command, "python ../run/gen_graph.py %s", data_file);
  printf("%s\n", command);
  int res = system(command);
  return res;
}
