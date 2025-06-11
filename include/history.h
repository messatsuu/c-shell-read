#ifndef PUBLIC_HISTORY_H
#define PUBLIC_HISTORY_H

extern int cshr_history_index;

void cshr_history_init(int initial_capacity);

void cshr_history_cleanup();

void chsr_history_append(const char *command);

char *cshr_history_get_command_dup(const unsigned int index);

char* chsr_history_get_command(unsigned long index);

void chsr_print_history();

#endif
