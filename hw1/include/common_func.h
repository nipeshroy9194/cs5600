/*
 * Filename : common_func.h
 * Description : Common functions used by both checkpoint and restart process
 *
 * Author : Nipesh Roy <roy.nip@husky.neu.edu>
 * Date : Sept 11 2017
 */
#include "memory_map.h"

#define CHECKPT_PATH "./myckpt"

/**
 * @brief : Process the command "cat /proc/self/maps" and redirect the output
 * 			to an unnamed pipe
 *
 * @param : pipe_fd - Unnamed Pipe File Descriptor
 *
 * @return : SUCCESS(0)/FAILURE(0)
 */
int process_command(int *pipe_fd);

/**
 * @brief : Create a process for cmd execution
 *
 * @param : pipe_fd	- Unnamed pipe File descriptor
 *
 * @return : SUCCESS(0)/FAILURE(1)
 */
int create_process_for_cmd_exec(int *pipe_fd);
