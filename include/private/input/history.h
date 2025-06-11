#ifndef HISTORY_H
#define HISTORY_H

typedef struct {
    char **entries; // Array of strings (dynamically allocated)
    unsigned int count;      // Number of current entries
    unsigned int capacity;   // Number of entries until reallocation is needed
} History;

#endif
