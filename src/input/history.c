#include "private/utility.h"
#include "private/input/history.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#define HISTORY_BUFFER_SIZE 20

// Declare a global variable to store history
History *history = NULL;

void history_init(int initial_capacity) {
    history = cshr_allocate(sizeof(History), true);
    history->history_entries = (HistoryEntry **)cshr_allocate(initial_capacity * sizeof(HistoryEntry *), true);
    history->count = 0;
    history->current_index = 1;
    history->capacity = initial_capacity;
    // Set the default max to around 4'294'967'295
    history->capacity_limit = UINT_MAX;

    if (!history->history_entries) {
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
        free(history->history_entries[i]->entry);
        free(history->history_entries[i]);
    }

    free(history->history_entries);
    free(history);
}

void cshr_history_append(const char *command) {
    if (history == NULL) {
        // On first call, we initialize the history-struct
        history_init(HISTORY_BUFFER_SIZE);
    } else if (history->count > 0 && strcmp(history->history_entries[history->count - 1]->entry, command) == 0) {
        // Do not append to history if the previous entry is the same as the current one
        return;
    }

    // If we reached the limit for the capacity, free the first entry and move all entries one to the left
    if (history->count >= history->capacity_limit) {
        free(history->history_entries[0]->entry);
        free(history->history_entries[0]);
        // clang throws warnings when passing multi-level pointers (e.g. (char **)) as a (void *), which is what memmove() expects.
        // We cast it manually to communicate intent.
        memmove(history->history_entries, history->history_entries + 1, (history->count - 1) * sizeof(HistoryEntry *));
        history->count--;
    }

    // If we reach the max of the currently allocated capacity, reallocate the the entries buffer
    if (history->count >= history->capacity) {
        history->capacity += HISTORY_BUFFER_SIZE;
        history->history_entries = (HistoryEntry **)cshr_reallocate(history->history_entries, (history->capacity) * sizeof(HistoryEntry *), true);
    }

    HistoryEntry *new_entry = cshr_allocate(sizeof(HistoryEntry), true);
    new_entry->entry = strdup(command);
    new_entry->index = history->current_index;
    history->history_entries[history->count] = new_entry;

    if (!new_entry->entry) {
        cshr_log_error_with_exit("Copying command string to buffer failed");
    }

    history->count++;
    history->current_index++;
}

char *cshr_history_get_entry(const unsigned int index) {
    if (history == NULL  || index == 0) {
        cshr_log_error("History index %lu out of range\n", index);
        return nullptr;
    }

    char *entry = nullptr;

    for (int i = 0; i < history->count; i++) {
        if (history->history_entries[i]->index == index) {
            entry = history->history_entries[i]->entry;
            break;
        }
    }

    if (entry == nullptr) {
        cshr_log_error("History index %lu out of range\n", index);
    }

    return entry;
}

char *cshr_history_get_entry_dup(const unsigned long index) {
    // we need to Duplicate the string, since making operations on it would change it in the history
    char *entry = cshr_history_get_entry(index);
    return entry == NULL ? entry : strdup(entry);
}

int cshr_print_history() {
    // If the history hasn't been initialized, return
    if (history == NULL) {
        printf("no history to print.");
        return 0;
    }

    for (int i = 0; i < history->count; i++) {
        HistoryEntry *history_entry = history->history_entries[i];
        printf("%d  %s\n", history_entry->index, history_entry->entry);
    }

    return 0;
}
