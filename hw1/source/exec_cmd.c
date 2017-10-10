/*
 * Filename : exec_cmd.c
 * Description : Program to execute a command using fork(),execvp(),waitpid()
 *
 * Author : Nipesh Roy <roy.nip@husky.neu.edu>
 * Date : Sept 11 2017
 */
#include "../include/helper_func.h"
#include "../include/memory_map.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int process_command(int *pipe_fd, int pid) {
  int ret = -1;
  char *arg[3] = {0}, *cmd = "cat", str_pid[100] = {'\0'};
  char str[512] = {'\0'};

  int_to_str(pid, str_pid);

  /**
   * "/proc/$pid/maps" contains the memory regions being currently used by
   * the process as well as the start and end address of all the memory
   * mappings currently in use by the process which will be then used for
   * checkpointing by acquiring the memory address and size and then storing
   * the data from the memory map into the checkpoint file.
   */
  strcpy(str, "/proc/");
  strcat(str, str_pid);
  strcat(str, "/maps");
  printf("%s", arg[1]);
  arg[0] = "cat";
  arg[1] = str;
  arg[2] = NULL;

  /* Close the read end of the unnamed pipe */
  ret = close(pipe_fd[0]);
  if (ret == -1) {
    printf("Unable to close read end of pipe : %d\n", errno);
    ret = FAILURE;
    goto error_out;
  }
  /**
   * Duplicate the file descriptors of the standard output as to
   * get the output in the unnamed pipe
   */
  ret = dup2(pipe_fd[1], 1); /* duplicate the std output */
  if (ret == -1) {
    printf("Unable to duplicate stdout FD : %d\n", errno);
    ret = FAILURE;
    goto error_out;
  }
  ret = dup2(pipe_fd[1], 2); /* duplicate the std error output */
  if (ret == -1) {
    printf("Unable to duplicate stderr FD : %d\n", errno);
    ret = FAILURE;
    goto error_out;
  }
  /* Close the write end of the unnamed pipe */
  ret = close(pipe_fd[1]);
  if (ret == -1) {
    printf("Unable to close write end of pipe : %d\n", errno);
    ret = FAILURE;
    goto error_out;
  }
  /* Execute the command in child process */
  ret = execvp(cmd, arg);
  if (ret == -1) {
    printf("Unable to process /proc/self/maps : %d\n", errno);
    ret = FAILURE;
    goto error_out;
  }

  ret = SUCCESS;
  return ret;

error_out:
  return ret;
}

int create_process_for_cmd_exec(int *pipe_fd) {
  int pid = 0, ret = -1, status = 0, ppid = 0;

  ret = pipe(pipe_fd);
  if (ret == -1) {
    printf("Unable to create an unnamed pipe :%d\n", errno);
    ret = FAILURE;
    goto error_out;
  }

  pid = 0;
  pid = fork();
  if (pid < 0) {
    printf("Unable to fork a child process :%d\n", errno);
    ret = FAILURE;
    goto error_out;
  } else if (pid == 0) {
    ppid = getppid();
    ret = process_command(pipe_fd, ppid);
    if (ret == FAILURE) {
      printf("Unable to process command\n");
      goto error_out;
    }
  } else {
    waitpid(pid, &status, 0);
  }

  ret = SUCCESS;
  return ret;

error_out:
  return ret;
}
