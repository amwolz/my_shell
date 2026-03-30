#include <main.h>
#include <signals.h>

int main(void) {

    // command variables;
    char cmdline[MAX_LINE];
    char *my_argv[MAX_ARGS];

    // redirect stderr to stdout
    dup2(1, 2);

    // setup signal listeners for process exits, interrupts, stops and quits
    setup_signals();

    // orphaned processes are reassigned to this (shell-project) pid group instead of pid group 1 init
    // this enables the shell to manage orphaned processes
    prctl(PR_SET_CHILD_SUBREAPER, 1);

    // prompt user
    while (1) {
        // input is from the live, interactive terminal. not an input file
        int is_tty = isatty(STDIN_FILENO);
        char *line;
        
        // interactive input
        if (is_tty) {
            line = readline("my_shell> ");
            if (line == NULL) {
                terminate_from_process(getpid());
                printf("logout\n");
                break;
            } else {
                HIST_ENTRY *prev = history_length > 0 ? history_get(history_base + history_length - 1) : NULL;
                if ((!prev || strcmp(prev->line, line) != 0) && !is_empty(line)) {
                    add_history(line);
                }
                strncpy(cmdline, line, sizeof(cmdline) - 1);
                free(line);
                parse(cmdline, my_argv);
                execute(my_argv);
            }
        // text file input
        } else {
            if (fgets(cmdline, sizeof(cmdline), stdin) == NULL) {
                printf("\n");
                break;
            }
            cmdline[strcspn(cmdline, "\n")] = '\0';
            line = cmdline;
            printf("my_shell> %s\n", line);
            parse(cmdline, my_argv);
            execute(my_argv);
        }

        print_done_jobs();
    }

    return 0;
}
