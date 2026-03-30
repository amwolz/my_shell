#include <parser.h>

// input string is empty or contains only spaces
int is_empty(char *line) {
    while (isspace(*line)) {
        line++;
    }
    return *line == '\0';
}

// parses input command line into argument array
void parse(char *line, char **my_argv) {
    int my_argc = 0;

    // convert to arguments
    while (*line != '\0') {
        while (*line && isspace((unsigned char)*line)) {
            line++;
        }
        if (*line == '\0') {
            break;
        }

        // add current argument to my_argv
        my_argv[my_argc++] = line;

        // null terminate current argument
        while (*line && !isspace((unsigned char)*line)) {
            line++;
        }
        if (*line != '\0') {
            *line = '\0';
            line++;
        }
    }

    // null terminate my_argv
    my_argv[my_argc] = NULL;
}
