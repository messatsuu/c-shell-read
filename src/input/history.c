#include "private/utility.h"

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
} History;

// Declare a global variable to store history
History *history = NULL;

int cshr_history_index = 0;

void cshr_history_init(int initial_capacity) {
    history = cshr_allocate(sizeof(History), true);
    history->entries = cshr_allocate(initial_capacity * sizeof(char *), true);
    history->count = 0;
    history->capacity = initial_capacity;

    if (!history->entries) {
        cshr_log_error_with_exit("History Allocation Error");
    }
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

void chsr_history_append(const char *command) {
    if (history == NULL) {
        // On first call, we initialize the history-struct
        cshr_history_init(HISTORY_BUFFER_SIZE);
    } else if (strcmp(history->entries[history->count - 1], command) == 0) {
        // Do not append to history if the previous entry is the same as the current one
        return;
    }

    // If we reach the max capacity, reallocate the the entries buffer
    if (history->count >= history->capacity) {
        history->capacity += HISTORY_BUFFER_SIZE;
        history->entries = cshr_recshr_allocate(history->entries, (history->capacity) * sizeof(char*), true);
    }

    // TODO: instead of using strdup, make sure that the original command is not
    history->entries[history->count] = strdup(command);
    if (!history->entries[history->count]) {
        cshr_log_error_with_exit("Copying command string to buffer failed");
    }

    history->count++;
}

char* cshr_history_get_command_dup(const unsigned long index) {
    if (history == NULL || index > history->count || index == 0) {
        cshr_log_error("History index %lu out of range\n", index);
        return nullptr;
    }

    unsigned int history_index = index;
    history_index--;

    char *command_from_history = history->entries[history_index];

    // we need to Duplicate the string, since making operations on it would change it in the history
    return strdup(command_from_history);
}

char *chsr_history_get_command(const unsigned int index) {
    return history->entries[index];
}

void chsr_print_history() {
    // If the history hasn't been initialized, return
    if (history == NULL) {
        return;
    }

    for (int i = 0; i < history->count; i++) {
        printf("%d  %s\n", i + 1, history->entries[i]);
    }
}
