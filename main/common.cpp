#include <ctime>

using namespace std;

void get_str_time(char *buffer, size_t sz) {
  /*
   * Code taken from:
   * http://stackoverflow.com/questions/5438482/getting-the-current-time-as-a-yyyy-mm-dd-hh-mm-ss-string
   */
  time_t rawtime;
  tm* timeinfo;
  
  time(&rawtime);
  timeinfo = localtime(&rawtime);
  
  strftime(buffer, sz, "%Y-%m-%d-%H-%M-%S", timeinfo);
}

int find_match_id(int *candidates, int len) {
  for(int i = 1; i <= len; i++) {
    if (candidates[i] == 0) {
      return i;
    }
  }

  return 0;
}
