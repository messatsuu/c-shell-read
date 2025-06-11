// TODO: this is a copy-paste from c-shell's utility module, we just prefix the functions to not get a duplicate symbol error
#ifndef UTILITY_H
#define UTILITY_H

#include <stdbool.h>
#include <stdio.h>

// Default Variables used across the platform
#define INITIAL_BUFSIZE 20
#define INITIAL_BUFSIZE_BIG 1024
#define BUF_EXPANSION_SIZE 100
#define BUF_EXPANSION_SIZE_BIG 1024
#define MAX_ARGUMENTS_SIZE 100

// Prints a message to stderr
void cshr_log_error(const char *format, ...);

// Prints a message to stderr and exits
void cshr_log_error_with_exit(const char *message);

// Wrapper around realloc that exits with a message to stderr when realloc fails
void *cshr_recshr_allocate(void *pointer, size_t size,  bool exit);

void *cshr_reallocate_safe(void *pointer, unsigned int old_size, unsigned int new_size, bool exit);

// Wrapper around malloc that exits with a message to stderr when malloc fails
void *cshr_allocate(size_t size, bool exit);

// Wrapper around calloc that exits with a message to stderr when calloc fails
void *cshr_callocate(unsigned int number_of_bytes, size_t size, bool exit);

// Simple function that calls all other cleanup functions
void cshr_cleanup();

ssize_t cshr_get_host_name(char *name, size_t len);

#endif
