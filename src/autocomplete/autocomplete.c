#define _GNU_SOURCE

#include "private/autocomplete/autocomplete.h"
#include "private/autocomplete/autocomplete_result.h"
#include "private/utility.h"

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void set_autocomplete_entries_directory(AutocompleteResult *autocompleteResult, char *path) {
    DIR *directory = opendir(path); // open the path
    if (directory == NULL) {
        return;
    }

    struct dirent * entry;
    while ((entry = readdir(directory)) != NULL)
    {
        // If the entry doesn't start with the search-term or is one of "." or "..", skip
        if (strncmp(entry->d_name, autocompleteResult->search_term, autocompleteResult->search_term_length) != 0 || strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        if (autocompleteResult->count >= autocompleteResult->capacity) {
            if (reallocate_autocomplete_entries(autocompleteResult, BUF_EXPANSION_SIZE) == -1) {
                closedir(directory);
                return;
            }
        }

        enum AutocompleteResultEntryType entryType = {0};
        if (entry->d_type == DT_DIR || entry->d_type == DT_BLK) {
            entryType = RESULT_ENTRY_TYPE_DIR;
        } else {
            entryType = RESULT_ENTRY_TYPE_FILE;
        }

        autocomplete_result_add_entry(autocompleteResult, entry->d_name, entryType);
    }

    // Sort entries by type
    qsort(autocompleteResult->entries, autocompleteResult->count, sizeof(AutocompleteResultEntry *), sort_autocomplete_result_entries);

    closedir(directory);
}

void autocomplete_path(AutocompleteResult *autocompleteResult) {
    // If the path contains a slash, we check if it is a directory
    char *last_slash = strrchr(autocompleteResult->search_term, '/');
    char *path = cshr_callocate(PATH_MAX, 1, true);

    if (last_slash != NULL) {
        unsigned int last_slash_position = last_slash - autocompleteResult->search_term + 1; // include the slash
        strncpy(path, autocompleteResult->search_term, last_slash_position);
        path[last_slash_position] = '\0';

        unsigned int new_search_term_length = autocompleteResult->search_term_length - last_slash_position;
        // If a path is found, we need to update the search-term to search for a specific directory instead (e.g. "/var/cac" => "cac")
        memmove(&autocompleteResult->search_term[0], &autocompleteResult->search_term[last_slash_position], new_search_term_length);
        autocompleteResult->search_term_length = new_search_term_length;
        autocompleteResult->search_term[autocompleteResult->search_term_length] = '\0';
    } else {
        path[0] = '.';
        path[1] = '\0';
    }

    set_autocomplete_entries_directory(autocompleteResult, path);
    free(path);
}

void set_search_term_from_input(InputBuffer *inputBuffer, AutocompleteResult *autocompleteResult) {
    unsigned int word_start_position = 0;
    char *before_cursor_buffer = cshr_callocate(inputBuffer->cursor_position + 1, 1, true);
    strncpy(before_cursor_buffer, inputBuffer->buffer, inputBuffer->cursor_position);
    before_cursor_buffer[inputBuffer->cursor_position] = '\0';

    // Find occurence of start of search-term (' ' + 1)
    char *last_space = strrchr(before_cursor_buffer, ' ');
    if (last_space != NULL) {
        word_start_position = last_space - before_cursor_buffer + 1; // +1 to move after the space
    }

    autocompleteResult->search_term_length = inputBuffer->cursor_position - (int)word_start_position;
    autocompleteResult->search_term = cshr_allocate(autocompleteResult->search_term_length + 1, true);
    memcpy(autocompleteResult->search_term, &inputBuffer->buffer[word_start_position], autocompleteResult->search_term_length);
    autocompleteResult->search_term[autocompleteResult->search_term_length] = '\0';
    free(before_cursor_buffer);
}

void set_autocomplete_entry_at_cursor_position(InputBuffer *inputBuffer, AutocompleteResult *autocompleteResult) {
    unsigned int result_length = strlen(autocompleteResult->entries[0]->entry) - autocompleteResult->search_term_length;
    char *result = autocompleteResult->entries[0]->entry + autocompleteResult->search_term_length;
    insert_into_buffer_at_cursor_position(inputBuffer, result, result_length);
}

void autocomplete(InputBuffer *inputBuffer) {
    AutocompleteResult autocompleteResult;

    init_autocomplete_result(&autocompleteResult);
    set_search_term_from_input(inputBuffer, &autocompleteResult);
    autocomplete_path(&autocompleteResult);

    // If we have only a single result, replace the search-term in the inputBuffer with the result
    if (autocompleteResult.count == 1) {
        set_autocomplete_entry_at_cursor_position(inputBuffer, &autocompleteResult);
    } else {
        print_autocomplete_entries(&autocompleteResult);
    }

    cleanup_autocomplete_result(&autocompleteResult);
}
