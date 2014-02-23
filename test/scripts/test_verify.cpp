#include "../../includes/verify_results.h"

int main(void) {
  char *matching[2] = {(char *)"matchOne.txt", (char *)"matchTwo.txt"};
  verify_results(2, matching);
}
