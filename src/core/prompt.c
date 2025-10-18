#include <string.h>
#include <termios.h>
#include <unistd.h>

unsigned int get_prompt_visible_length(const char *prompt) {
    int len = 0;
    for (const char *p = prompt; *p; p++) {
        // Skip prompt titles
        if (strncmp(p, "\x1B]0;", 4) == 0) {
            while (*p && *p != '\a') p++;
            continue;
        }

        // Skip escape sequences
        if (*p == '\033') {
            while (*p && *p != 'm') p++;
            continue;
        }

        len++;
    }
    return len;
}
