#include "private/utility.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>
#include <unistd.h>

__attribute__((format(printf, 1, 2)))
void cshr_log_error(const char *format, ...) {
    va_list args;
    va_start(args, format);

    // NOTE: Can this cause issues if the called function did not write to perror?
    perror("");
    if (vfprintf(stderr, format, args) < 0) {
        perror("Error writing to stderr");
    }

    printf("\n");
    va_end(args);
}

noreturn void cshr_log_error_with_exit(const char *message) {
    cshr_log_error("%s", message);
    exit(EXIT_FAILURE);
}

void *cshr_reallocate(void *pointer, size_t size, bool exit) {
    void *reallocation_result = realloc(pointer, size);
    if (!reallocation_result) {
        free(pointer);
        if (exit) {
            cshr_log_error_with_exit("Reallocation Error");
        }
        return NULL;
    }

    return reallocation_result;
}

// Calls realloc() and initializes the added memory with calloc()
void *cshr_reallocate_safe(void *pointer, unsigned int old_size, unsigned int new_size, bool exit) {
    void *reallocation_result = realloc(pointer, new_size);
    if (!reallocation_result) {
        free(pointer);
        if (exit) {
            cshr_log_error_with_exit("Reallocation Error");
        } else {
            cshr_log_error_with_exit("Reallocation Error");
        }
        return NULL;
    }

    if (new_size > old_size) {
        memset((char *)reallocation_result + old_size, 0, new_size - old_size);
    }

    return reallocation_result;
}

void *cshr_allocate(size_t size, bool exit) {
    void *allocation_result = malloc(size);
    if (!allocation_result) {
        if (exit) {
            cshr_log_error_with_exit("Allocation Error");
        }
        return NULL;
    }

    return allocation_result;
}

void *cshr_callocate(unsigned int number_of_bytes, size_t size, bool exit) {
    void *callocation_result = calloc(number_of_bytes, size);
    if (!callocation_result) {
        if (exit) {
            cshr_log_error_with_exit("Zero-Value Allocation Error");
        }
        return NULL;
    }

    return callocation_result;
}
