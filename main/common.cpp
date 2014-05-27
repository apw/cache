#include "../includes/common.h"

#include "stdio.h"

#include <ctime>
#include <iostream>

#include <assert.h>

using namespace std;

char *get_hex(unsigned x) {
  assert(x <= 0xff);
  unsigned small_digit = x % 16;
  unsigned large_digit = (x - small_digit) / 16;
  char *ret;
  int num = asprintf(&ret, "%x%x", large_digit, small_digit);
  assert(num == sizeof(char) * 2);
  return ret;
}


void get_str_time(char *buffer, size_t sz) {
/*
   * Code taken from:
   * http://stackoverflow.com/questions/5438482/getting-the-current-time-as-a-yyyy-mm-dd-hh-mm-ss-string
   */
  time_t rawtime;
  tm* timeinfo;
  
  time(&rawtime);
  // the following call leaks memory but we are not allowed to free it
  timeinfo = localtime(&rawtime);
  assert(timeinfo != NULL);
  
  strftime(buffer, sz, "%Y-%m-%d-%H-%M-%S", timeinfo);
}

void pterodactyl() {
  // http://ascii.co.uk/art/dinosaur
  printf("\n");
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

void bears() {
  // http://www.retrojunkie.com/asciiart/animals/bears.htm
  printf("\n");
  printf("              {\"`-'\"}                \n");
  printf("  {\"`-'\"}      (o o)      {\"`-'\"}    \n");
  printf("   (o o)       ,`Y'.       (o o)     \n");
  printf(" ,--`Y'--.    / ,-. \\    ,--`Y'--.   \n");
  printf(" ``:   ;''   (_)| |(_)   `-:,-.;-'   \n");
  printf("   / _ \\       /`_'\\       /`_'\\     \n");
  printf("  ()' `()     (_) (_)     ()' `()    \n");
  printf("^'^'^^^'^'`^^`^```^``'^^``^^```'^'^\n");

}

void forest() {
  // http://www.retrojunkie.com/asciiart/nature/forests.htm
  printf("                         /\\     |\\             /  \n");
  printf("            /   /|      / \\    / \\  \\   \\     /|  \n");
  printf("           / \\ / \\  /    |     / \\ / \\ / \\    / \\ \n");
  printf("           / \\ / \\ / \\  \\   /   |  / \\ / \\   \\ | \n");
  printf("            |   |  / \\ / \\ / \\   /  |   |   / \\     \\ \n");
  printf("                \\   |  / \\ / \\  / \\  \\     \\/ \\    / \\ \n");
  printf("               / \\      |   |   / \\ / \\   / \\|  /  / \\ \n");
  printf("               / \\    /    \\     |  / \\   / \\  / \\  | \n");
  printf("                |    / \\  / \\        |     |   / \\ \n");
  printf("                     / \\  / \\                   | \n");
  printf("                      |    |       \n");
}

void snorlax() {
  // http://kodojoe.deviantart.com/art/Snorlax-text-art-307671909
  printf("        ^ ^  \n");
  printf("    (\"\\(-_-)/\")  \n");
  printf("       (( ))  \n");
  printf("     ((.) (.))  \n");
}
