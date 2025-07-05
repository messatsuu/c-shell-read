#ifndef PUBLIC_HISTORY_H
#define PUBLIC_HISTORY_H

void cshr_history_cleanup();

void cshr_history_append(const char *command);

// Get command at index. Note that mutations will mutate the string in the history.
char *cshr_history_get_entry(unsigned long index);

// Get command at index as a strdup'd string.
char *cshr_history_get_entry_dup(unsigned int index);

// Set a limit on how many history entries are persisted
void cshr_set_history_limit(unsigned int limit);

void cshr_print_history();

#endif
