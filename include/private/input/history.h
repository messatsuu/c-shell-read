#ifndef HISTORY_H
#define HISTORY_H

typedef struct {
    unsigned int index;
    char *entry;
} HistoryEntry;

typedef struct {
    HistoryEntry **history_entries; // Array of history entries (dynamically allocated)
    unsigned int count;      // Number of current entries
    unsigned int current_index; // The current index, internal counter for number of entries that have been added in the past
    unsigned int capacity;   // Number of entries until reallocation is needed
    unsigned int capacity_limit;   // Max number of entries until oldest entry get deallocated
} History;

#endif
