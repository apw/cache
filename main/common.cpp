#include "../includes/common.h"
#include <ctime>
#include "stdio.h"

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

void pterodactyl() {
  printf("                               <\\              _                   \n");
  printf("                                \\\\          _/{                   \n");
  printf("                         _       \\\\       _-   -_                 \n");
  printf("                      /{        / `\\   _-     - -_               \n");
  printf("                    _~  =      ( @  \\ -        -  -_             \n");
  printf("                  _- -   ~-_   \\( =\\ \\           -  -_           \n");
  printf("                _~  -       ~_ | 1 :\\ \\      _-~-_ -  -_         \n");
  printf("              _-   -          ~  |V: \\ \\  _-~     ~-_-  -_       \n");
  printf("           _-~   -            /  | :  \\ \\            ~-_- -_   \n");
  printf("        _-~    -   _.._      {   | : _-``               ~- _-_ \n");
  printf("     _-~   -__..--~    ~-_  {   : \\:}                         \n");
  printf("    =~__.--~~              ~-_\\  :  /                          \n");
  printf("                               \\ : /__                         \n");
  printf("                              //`Y'--\\\\                         \n");
  printf("                             <+       \\\\                      \n");
  printf("                              \\\\      WWW                     \n");
  printf("                              MMM                             \n");
}
