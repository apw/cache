#include "../includes/common.h"
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
