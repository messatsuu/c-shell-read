#ifndef PUBLIC_INPUT_H
#define PUBLIC_INPUT_H

#include <signal.h>

extern volatile sig_atomic_t cshr_sigint_received;

// Reads input from the user
char *cshr_read_input(char *prompt);

#endif
