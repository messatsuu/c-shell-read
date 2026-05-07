#ifndef AUTOCOMPLETE_RESULTH_H
#define AUTOCOMPLETE_RESULTH_H

#define ENTRY_MAX 4096

#define AUTOCOMPLETE_COLOR_BLUE "\033[0;34m"
#define AUTOCOMPLETE_COLOR_WHITE "\033[37m"
#define AUTOCOMPLETE_COLOR_NORMAL "\033[0m"

typedef struct {
    char *entry;
    char *color;
} AutocompleteResultEntry;

typedef struct {
    AutocompleteResultEntry **entries;                 // Array of strings (dynamically allocated)
    unsigned int count;             // Number of entries
    unsigned int capacity;          // Current capacity
    char *search_term;              // The term to filter by
    unsigned int search_term_length; // length of the search-term
} AutocompleteResult;

void init_autocomplete_result(AutocompleteResult *autocompleteResult);

int autocomplete_result_add_entry(AutocompleteResult *autocompleteResult, char *entry, char* color);

int reallocate_autocomplete_entries(AutocompleteResult *autocompleteResult, unsigned int entries_expansion_size);

void cleanup_autocomplete_result(AutocompleteResult *autocompleteResult);

void print_autocomplete_entries(AutocompleteResult *autocompleteResult);

#endif
