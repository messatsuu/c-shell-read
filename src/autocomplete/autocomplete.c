#include "private/autocomplete/autocomplete.h"
#include "private/autocomplete/autocomplete_result.h"
#include "private/autocomplete/sources/sources_path.h"
#include "private/utility.h"
#include <dirent.h>
#include <stdlib.h>
#include <string.h>

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
    autocomplete_source_path(&autocompleteResult);

    // If we have only a single result, replace the search-term in the inputBuffer with the result
    if (autocompleteResult.count == 1) {
        set_autocomplete_entry_at_cursor_position(inputBuffer, &autocompleteResult);
    } else {
        print_autocomplete_entries(&autocompleteResult);
    }

    cleanup_autocomplete_result(&autocompleteResult);
}
