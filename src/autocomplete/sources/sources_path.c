#include <stdio.h>
#define _GNU_SOURCE

#include "private/autocomplete/sources/sources_path.h"
#include "private/autocomplete/autocomplete_result.h"
#include "private/utility.h"
#include <dirent.h>
#include <linux/limits.h>
#include <stdlib.h>
#include <string.h>

int sort_directory_autocomplete(const void *a, const void *b) {
    AutocompleteResultEntry *entry1 = *(AutocompleteResultEntry **)a;
    AutocompleteResultEntry *entry2 = *(AutocompleteResultEntry **)b;

    // Sort directories first
    if (entry1->entry[strlen(entry1->entry) - 1] == '/') {
        return -1;
    }
    if (entry2->entry[strlen(entry2->entry) - 1] == '/') {
        return 1;
    }

    return strcmp(entry2->entry, entry2->entry);
}

void set_autocomplete_entries_directory(AutocompleteResult *autocompleteResult, char *path) {
    DIR *directory = opendir(path);
    if (directory == NULL) {
        closedir(directory);
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(directory)) != NULL) {
        // If the entry doesn't start with the search-term or is one of "." or "..", skip
        if (strncmp(entry->d_name, autocompleteResult->search_term, autocompleteResult->search_term_length) != 0 || strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char *entry_value = cshr_callocate(ENTRY_MAX, 1, true);
        const char *autocomplete_color = AUTOCOMPLETE_COLOR_NORMAL;

        // If it's a directory or block-device, add a '/' at the end, else just copy
        if (entry->d_type == DT_DIR || entry->d_type == DT_BLK) {
            snprintf(entry_value, ENTRY_MAX, "%s/", entry->d_name);
            autocomplete_color = AUTOCOMPLETE_COLOR_BLUE;
        } else {
            strcpy(entry_value, entry->d_name);
        }

        if (autocomplete_result_add_entry(autocompleteResult, entry_value, (char *)autocomplete_color) == -1) {
            closedir(directory);
            cshr_log_error_with_exit("CSHR: error while adding autocomplete entry");
        }

        free(entry_value);
    }

    // Sort entries by type
    qsort((void *)autocompleteResult->entries, autocompleteResult->count, sizeof(AutocompleteResultEntry *), sort_directory_autocomplete);

    closedir(directory);
}

void autocomplete_source_path(AutocompleteResult *autocompleteResult) {
    // If the path contains a slash, we check if it is a directory
    char *last_slash = strrchr(autocompleteResult->search_term, '/');
    char *path = cshr_callocate(PATH_MAX, 1, true);

    if (last_slash != nullptr) {
        unsigned int last_slash_position = last_slash - autocompleteResult->search_term + 1; // include the slash
        strncpy(path, autocompleteResult->search_term, last_slash_position);
        path[last_slash_position] = '\0';

        unsigned int new_search_term_length = autocompleteResult->search_term_length - last_slash_position;
        // If a path is found, we need to update the search-term to search for a specific directory instead
        // Passed search-term: "/var/cac"
        // => path     = "/var/"
        // => search-term   = "cac"
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

