/*
 * Filename : memory_map.c
 * Description : Process and write memory maps
 *
 * Author : Nipesh Roy <roy.nip@husky.neu.edu>
 * Date : Sept 11 2017
 */
#include "../include/memory_map.h"
#include "../include/helper_func.h"
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void populate_mem_reg(mem_reg *mr, char *memory_map) {
  char str1[256] = {'\0'}, str2[256] = {'\0'};
  char str3[256] = {'\0'}, str4[256] = {'\0'};
  char str5[256] = {'\0'}, str6[256] = {'\0'};
  char *token = NULL;
  char *end_ptr = NULL;
  unsigned long int start_addr = 0, end_addr = 0;
  char *buf = calloc(1, 512);
  char *buf1 = calloc(1, 512);

  sscanf(memory_map, "%s %s %s %s %s %s", str1, str2, str3, str4, str5, str6);

  /* process str1 to get the start and end offset */
  token = strtok(str1, "-");
  memcpy(buf, token, strlen(token));
  start_addr = strtol(buf, &end_ptr, 16);

  token = strtok(NULL, "-");
  memcpy(buf1, token, strlen(token));
  end_addr = strtol(buf1, &end_ptr, 16);

  mr->start_addr = (void *)start_addr;
  mr->end_addr = (void *)end_addr;
  mr->total_data_size = end_addr - start_addr;

  mr->is_readable = 0;
  mr->is_writeable = 0;
  mr->is_executable = 0;
  mr->is_private = 0;

  /* process str2 to get the permissions */
  if (str2[0] == 'r')
    mr->is_readable = 1;
  if (str2[1] == 'w')
    mr->is_writeable = 1;
  if (str2[2] == 'x')
    mr->is_executable = 1;
  if (str2[3] == 'p')
    mr->is_private = 1;

  mr->filename = calloc(1, 512);
  memcpy(mr->filename, str6, sizeof(str6));
}

int write_mem_ref(mem_reg *mr, int ckpt_fd) {
  int ret = -1;

  /* To skip private guard regions */
  if (mr->is_readable || mr->is_writeable || mr->is_executable) {
    ret = write(ckpt_fd, mr, sizeof(*mr));
    if (ret == -1) {
      printf("Write failed : %d\n", errno);
      ret = FAILURE;
      goto error_out;
    }

    ret = write(ckpt_fd, mr->start_addr, mr->total_data_size);
    if (ret == -1) {
      printf("Data Write failed : %d\n", errno);
      ret = FAILURE;
      goto error_out;
    }
  }

  ret = SUCCESS;
  return ret;

error_out:
  return ret;
}

int process_memory_map(mem_reg *mr, char *memory_map, int *pipe_fd,
                       int ckpt_fd) {
  char ch = '\0';
  int itr = 0, ret = -1;

  /**
   * Close the write end of the unnamed pipe on the parent process as there is
   * nothing to write now
   */
  ret = close(pipe_fd[1]);
  if (ret == -1) {
    printf("Unable to close write end of pipe on parent : %d\n", errno);
    ret = FAILURE;
    goto error_out;
  }

  /**
   * Read the memory map array from the command output and parse the output
   * into the memory mapped structure
   */
  itr = 0;
  while (read(pipe_fd[0], &ch, 1) != 0) {
    if (ch == '\n') {
      memory_map[itr] = '\0';
      populate_mem_reg(mr, memory_map);
      if (strstr(mr->filename, "[vsyscall]") != NULL) {
        continue;
      }
      ret = write_mem_ref(mr, ckpt_fd);
      if (ret == FAILURE) {
        printf("Unable to write memory-map to the file : %d\n", errno);
        goto error_out;
      }
      itr = 0;
      memset(memory_map, 0, sizeof(*memory_map));
    } else {
      memory_map[itr++] = ch;
    }
  }

  /* Close the read end of the unnamed pipe */
  ret = close(pipe_fd[0]);
  if (ret == -1) {
    printf("Unable to close read end of the pipe : %d\n", errno);
    ret = FAILURE;
    goto error_out;
  }

  ret = SUCCESS;
  return ret;

error_out:
  return ret;
}
