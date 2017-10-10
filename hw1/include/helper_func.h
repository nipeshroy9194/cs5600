/*
 * Filename : helper_func.h
 * Description : Helper functions used by both checkpoint and restart process
 *
 * Author : Nipesh Roy <roy.nip@husky.neu.edu>
 * Date : Sept 11 2017
 */
#include <stdio.h>

/**
 * @brief : Helper function to reverse a string
 *
 * @param : buffer - The string to be reversed
 */
void str_reverse(char *buffer);

/**
 * @brief : Integer to string converter
 *
 * @param : pid - The pid to be converted
 * @param : buffer - The string obtained after conversion
 */
void int_to_str(int pid, char *buffer);