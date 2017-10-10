/*
 * Filename : memory_map.h
 * Description : Header including everything related to memory maps
 *
 * Author : Nipesh Roy <roy.nip@husky.neu.edu>
 * Date : Sept 11 2017
 */
#include <stdio.h>
#include <ucontext.h>

#define SUCCESS 0
#define FAILURE 1

typedef struct memory_region {
  /* Start address & End address of the memory region */
  void *start_addr;
  void *end_addr;
  /* Permissions for the particular memory region */
  int is_readable;
  int is_writeable;
  int is_executable;
  int is_private;
  /**
   * It represent the filename from where the memory is populated
   * say for 'cat' the memory is populated from the 'usr/bin/cat' file
   */
  void *filename;
  /* The total data size of the memory region */
  int total_data_size;
} mem_reg;

/**
 * @brief : Populate Memory Region structure
 *
 * @param : mr         - Memory Map structure to be populated
 * @param : memory_map - Memory Map Buffer from /proc/pid/maps
 *
 * @return : SUCCESS(0)/FAILURE(1)
 */
void populate_mem_reg(mem_reg *mr, char *memory_map);

/**
 * @brief : Write memory map into the file
 *
 * @param : mr - Memory map structure to be written
 * @param : ckpt_fd - File descriptor of the file which contains checkpoint
 * 					  data
 *
 * @return : SUCCESS(0)/FAILURE(1)
 */
int write_mem_ref(mem_reg *mr, int ckpt_fd);

/**
 * @brief : Process the memory map and checkpoint the data
 *
 * @param : mr         - Memory map structure
 * @param : memory_map - Memory map read from "/proc/self/maps"
 * @param : pipe_fd    - Unnamed Pipe File Descriptor
 * @param : ckpt_fd  - Checkpoint file, File Descriptor
 *
 * @return : SUCCESS(0)/FAILURE(0)
 */
int process_memory_map(mem_reg *mr, char *memory_map, int *pipe_fd,
                       int ckpt_fd);
