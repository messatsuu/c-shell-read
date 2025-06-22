#include "private/utility.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#define HISTORY_BUFFER_SIZE 20

typedef struct {
    char **entries; // Array of strings (dynamically allocated)
    unsigned int count;      // Number of current entries
    unsigned int capacity;   // Number of entries until reallocation is needed
    unsigned int capacity_limit;   // Max number of entries until oldest entry get deallocated
} History;

// Declare a global variable to store history
History *history = NULL;

int cshr_history_index = 0;

void history_init(int initial_capacity) {
    history = cshr_allocate(sizeof(History), true);
    history->entries = (char **)cshr_allocate(initial_capacity * sizeof(char *), true);
    history->count = 0;
    history->capacity = initial_capacity;
    // Set the default max to around 4'294'967'295
    history->capacity_limit = UINT_MAX;

    if (!history->entries) {
        cshr_log_error_with_exit("History Allocation Error");
    }
}

void cshr_set_history_limit(unsigned int limit) {
    if (history == NULL) {
        history_init(HISTORY_BUFFER_SIZE);
    }

    history->capacity_limit = limit;
}

void cshr_history_cleanup() {
    if (history == NULL) {
        return;
    }

    for (int i = 0; i < history->count; i++) {
        free(history->entries[i]);
    }

    free(history->entries);
    free(history);
}

void cshr_history_append(const char *command) {
    if (history == NULL) {
        // On first call, we initialize the history-struct
        history_init(HISTORY_BUFFER_SIZE);
    } else if (history->count > 0 && strcmp(history->entries[history->count - 1], command) == 0) {
        // Do not append to history if the previous entry is the same as the current one
        return;
    }

    // If we reached the limit for the capacity, free the first entry and move all entries one to the left
    if (history->count >= history->capacity_limit) {
        free(history->entries[0]);
        // clang throws warnings when passing multi-level pointers (e.g. (char **)) as a (void *), which is what memmove() expects.
        // We cast it manually to communicate intent.
        memmove(history->entries, history->entries + 1, (history->count - 1) * sizeof(char *));
        history->count--;
    }

    // If we reach the max of the currently allocated capacity, reallocate the the entries buffer
    if (history->count >= history->capacity) {
        history->capacity += HISTORY_BUFFER_SIZE;
        history->entries = (char **)cshr_reallocate(history->entries, (history->capacity) * sizeof(char*), true);
    }

    // TODO: instead of using strdup, make sure that the original command is not
    history->entries[history->count] = strdup(command);
    if (!history->entries[history->count]) {
        cshr_log_error_with_exit("Copying command string to buffer failed");
    }

    history->count++;
}

char *cshr_history_get_entry(const unsigned int index) {
    if (history == NULL || index > history->count || index == 0) {
        cshr_log_error("History index %lu out of range\n", index);
        return nullptr;
    }

    unsigned int history_index = index;
    history_index--;

    return history->entries[history_index];
}

char *cshr_history_get_entry_dup(const unsigned long index) {
    // we need to Duplicate the string, since making operations on it would change it in the history
    return strdup(cshr_history_get_entry(index));
}

void cshr_print_history() {
    // If the history hasn't been initialized, return
    if (history == NULL) {
        return;
    }

    for (int i = 0; i < history->count; i++) {
        printf("%d  %s\n", i + 1, history->entries[i]);
    }
}
