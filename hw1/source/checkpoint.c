/*
 * Filename : checkpoint.c
 * Description : Program to checkpoint a process
 *
 * Author : Nipesh Roy <roy.nip@husky.neu.edu>
 * Date : Sept 11 2017
 */
#include "../include/common_func.h"
#include "../include/helper_func.h"
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int flag = 0;

/**
 * @brief : Write the process context into the checkpoint file
 *
 * @param : process_ctx - The structure storing the entire process context
 * @param : ckpt_fd 	- checkpoint file FD
 *
 * @return : SUCCESS(0)/FAILURE(1)
 */
int write_ctx_to_ckpt_header(ucontext_t *process_ctx, int ckpt_fd) {
  int ret = -1;

  /* Write the process context for restarting */
  ret = write(ckpt_fd, process_ctx, sizeof(*process_ctx));
  if (ret == -1) {
    printf("Write failed : %d\n", errno);
    ret = FAILURE;
    goto error_out;
  }
  ret = SUCCESS;
  return ret;

error_out:
  return ret;
}

/**
 * @brief : Function which initiates the checkpoint process
 *
 * @return : SUCCESS(0)/FAILURE(1)
 */
void start_checkpoint() {
  int pipe_fd[2] = {0}, ret = -1;
  char memory_map[512] = {'\0'};
  mem_reg *mr = NULL;
  int ckpt_fd = 0;
  ucontext_t process_ctx;

  /* Initialise Memory path structure */
  mr = (mem_reg *)calloc(1, sizeof(*mr));
  if (mr == NULL) {
    printf("Unable to allocate memory\n");
    ret = FAILURE;
    goto error_out;
  }

  /* Open the file where the checkpoint will be created */
  ckpt_fd = open(CHECKPT_PATH, (O_RDWR | O_CREAT | O_TRUNC), (S_IRWXU));
  if (ckpt_fd == -1) {
    printf("Unable to open checkpoint file : %d\n", errno);
    ret = FAILURE;
    goto error_out;
  }

  /* Read the cat /proc/pid/maps */
  ret = create_process_for_cmd_exec(pipe_fd);
  if (ret == FAILURE) {
    printf("Unable to execute command to read process memory map\n");
    goto error_out;
  }

  /* Get the process context */
  ret = getcontext(&process_ctx);
  if (ret == -1) {
    printf("Unable to get process context : %d\n", errno);
    ret = FAILURE;
    goto error_out;
  }

  if (flag == 1) {
    return;
  }

  flag = 1;

  /* Write the context to the checkpoint file */
  ret = write_ctx_to_ckpt_header(&process_ctx, ckpt_fd);
  if (ret == FAILURE) {
    printf("Unable to get write context to checkpoint file\n");
    goto error_out;
  }

  /* Process memory map and write it to the checkpoint file */
  ret = process_memory_map(mr, memory_map, pipe_fd, ckpt_fd);
  if (ret == FAILURE) {
    printf("Unable to process memory maps\n");
    goto error_out;
  }

  ret = close(ckpt_fd);
  if (ret == FAILURE) {
    printf("Unable to close the checkpoint file FD : %d\n", errno);
    goto error_out;
  }

  free(mr);
  mr = NULL;

  printf("Process Checkpointed successfully\n");
  ret = SUCCESS;
  exit(ret);

error_out:
  exit(ret);
}

/**
 * @brief : Signal Handler
 *
 * @param : signum - Signal number that is caught
 */
void handle_signal(int signum) {
  /* Start the checkpointing signal when you receive SIGUSR2 */
  start_checkpoint();
}

/**
 * @brief : Constructor to initialise signal handler
 */
__attribute__((constructor)) void sig_handler_init() {
  int ret = -1;
  struct sigaction catch_sig = {0};

  catch_sig.sa_handler = &handle_signal;
  catch_sig.sa_flags = SA_SIGINFO;

  ret = sigaction(SIGUSR2, &catch_sig, NULL);
  if (ret == -1) {
    printf("Unable to initialise signal handler : %d", errno);
    exit(FAILURE);
  }
}
