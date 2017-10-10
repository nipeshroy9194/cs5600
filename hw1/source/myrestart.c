/*
 * Filename : myrestart.c
 * Description : Program to restore a process using a checkpoint image
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
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <ucontext.h>
#include <unistd.h>

char ckpt_filename[1000];

/**
 * @brief : Process memory map for the restart program
 *
 * @param : mr 		   - Memory map structure
 * @param : memory_map - String containing memory map from /proc/self/maps
 * @param : pipe_fd    - Unnamed pipe fd
 *
 * @return : SUCCESS(0)/FAILURE(1)
 */
int proc_memory_map(mem_reg *mr, char *memory_map, int *pipe_fd) {
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
      if (ret == FAILURE) {
        printf("Unable to write memory-map to the file : %d\n", errno);
        goto error_out;
      }
      if (strncmp(mr->filename, "[stack]", 7) == 0) {
        break;
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

/**
 * @brief : Read ckpt file and restore memory maps
 *
 * @param : proc_ctx - Process context of checkpointed file
 * @param : ckpt_filename - Checkpoint filename
 * @param : mr - Memory map structure
 *
 */
int read_ckpt_file_and_restore(ucontext_t *proc_ctx, char *ckpt_filename,
                               mem_reg *mr) {
  int ckpt_fd = 0, flags = 0, prot = 0, ret = -1;
  void *addr = NULL;
  unsigned long int start_addr = 0;
  void *buf = NULL;

  /* Open the checkpoint FD */
  ckpt_fd = open(CHECKPT_PATH, O_RDWR, S_IRWXU);
  if (ckpt_fd == -1) {
    printf("Unable to open the checkpoint file :%d\n", errno);
    ret = FAILURE;
    goto error_out;
  }

  /* Read the process context */
  ret = read(ckpt_fd, proc_ctx, sizeof(*proc_ctx));
  if (ret < 0) {
    printf("Unable to read from the checkpoint file :%d\n", errno);
    ret = FAILURE;
    goto error_out;
  }

  while (1) {
    /* Read the memory map metadata structure */
    ret = read(ckpt_fd, mr, sizeof(*mr));
    if (ret == -1) {
      printf("Unable to read from the checkpoint file :%d\n", errno);
      ret = FAILURE;
      goto error_out;
    }

    if (mr->start_addr == NULL)
      break;

    if (mr->is_readable == 1)
      prot |= PROT_READ;
    /**
     * Add write permission to every memory map as we need to write that
     * memory area with the data during restore
     */
    if (mr->is_writeable == 1 || mr->is_writeable == 0)
      prot |= PROT_WRITE;
    if (mr->is_executable == 1)
      prot |= PROT_EXEC;
    if (mr->is_private == 1)
      flags |= MAP_PRIVATE;

    flags |= (MAP_ANONYMOUS | MAP_FIXED);

    /* Memory Map the processes map at its original position */
    addr = mmap(mr->start_addr, mr->total_data_size, prot, flags, -1, 0);
    if (addr == MAP_FAILED) {
      printf("Unable to mmap: %d\n", errno);
      ret = FAILURE;
      goto error_out;
    }

    buf = calloc(1, mr->total_data_size);
    if (buf == NULL) {
      printf("Unable to allocate memory\n");
      ret = FAILURE;
      goto error_out;
    }

    /**
     * Read the total data related to the particular memory map being
     * processed
     */
    ret = read(ckpt_fd, buf, mr->total_data_size);
    if (ret == -1) {
      printf("Unable to read from the checkpoint file :%d\n", errno);
      ret = FAILURE;
      goto error_out;
    }

    memcpy(mr->start_addr, buf, mr->total_data_size);

    free(buf);
    buf = NULL;
  }

  ret = close(ckpt_fd);
  if (ret == -1) {
    printf("Unable to close checkpoint file :%d\n", errno);
    ret = FAILURE;
    goto error_out;
  }

  ret = SUCCESS;
  return ret;

error_out:
  return ret;
}

/**
 * @brief : Restore memory maps
 */
void restore_memory_maps() {
  int pipe_fd[2] = {0}, ret = -1;
  char memory_map[512] = {'\0'};
  int start_addr = 0, end_addr = 0;
  int stack_len = 0;
  void *buf = NULL;
  mem_reg *mr = NULL;
  ucontext_t proc_ctx;

  /* Allocate memory for the memory register */
  mr = (mem_reg *)calloc(1, sizeof(*mr));
  if (mr == NULL) {
    printf("Unable to allocate memory\n");
	return;
  }

  /* Read the restart processes memory map */
  ret = create_process_for_cmd_exec(pipe_fd);
  if (ret == FAILURE) {
    printf("Unable to execute command to read process memory map\n");
	return;
  }

  /**
   * Process the memory map of the restart process to get current stack
   * address
   */
  ret = proc_memory_map(mr, memory_map, pipe_fd);
  if (ret == FAILURE) {
    printf("Unable to process memory maps\n");
	return;
  }

  stack_len = mr->total_data_size;

  /**
   * Munmap fails with EINVAL if there is an memory leak hence ignoring
   * munmap return value in this case
   */
  ret = munmap(mr->start_addr, stack_len);
  if (ret == -1) {
    printf("Unable to munmap : %d\n", errno);
	return;
  }

  /* Restore the checkpointed processes memory map */
  ret = read_ckpt_file_and_restore(&proc_ctx, CHECKPT_PATH, mr);
  if (ret == FAILURE) {
    printf("Unable to read and restore the ckpt process\n");
	return;
  }

  free(mr);
  mr = NULL;

  setcontext(&proc_ctx);
}

int main(int argc, char **argv) {
  unsigned long int start_addr = 0x5300000;
  void *stack_new_addr = (void *)start_addr, *stack_addr = NULL;

  if (argc == 1) {
    printf("Pass a checkpoint file path\n");
    exit(FAILURE);
  }

  strncpy(ckpt_filename, argv[1], strlen(argv[1]));

  /* Allocate memory for the new stack */
  stack_addr = mmap(stack_new_addr, 8192, (PROT_READ | PROT_WRITE),
                    (MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED), -1, 0);
  if (stack_addr == MAP_FAILED) {
    printf("Unable to mmap : %d\n", errno);
    return FAILURE;
  }

  /* Move the stack pointer to new address */
  start_addr = start_addr + 8192;
  asm volatile("mov %0,%%rsp" : : "g"(start_addr) : "memory");

  /* Restore the memory maps from the checkpoint */
  restore_memory_maps();
}
