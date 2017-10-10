/*
 * Filename : hello.c
 * Description : Sample program to be checkpointed
 *
 * Author : Nipesh Roy <roy.nip@husky.neu.edu>
 * Date : Sept 11 2017
 */
#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv) {
  while (1) {
    printf(".");
    fflush(stdout);
    sleep(1);
  }
}