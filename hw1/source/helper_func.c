/*
 * Filename : helper_func.c
 * Description : Contains helper functions
 *
 * Author : Nipesh Roy <roy.nip@husky.neu.edu>
 * Date : Sept 11 2017
 */
#include "../include/helper_func.h"
#include "stdlib.h"
#include "string.h"

void str_reverse(char *buffer) {
  int var = 0, i = 0, j = strlen(buffer) - 1;

  while (i < j) {
    var = buffer[i];
    buffer[i] = buffer[j];
    buffer[j] = var;
    i++;
    j--;
  }
}

void int_to_str(int pid, char *buffer) {
  int rem = -1, i = 0;

  while (pid != 0) {
    rem = pid % 10;
    pid = pid / 10;
    buffer[i] = rem + 48;
    i++;
  }
  str_reverse(buffer);
}
