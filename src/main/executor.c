#include <executor.h>

// adds a job to jobs with jid, pid, state and command
void add_job(pid_t pid, int jid, char *cmd, int background) {
    jobs[jid - 1].pid = pid;
    jobs[jid - 1].jid = jid;
    jobs[jid - 1].state = background ? BG : FG;
    strncpy(jobs[jid - 1].cmdline, cmd, MAX_LINE - 1);
    jobs[jid - 1].cmdline[MAX_LINE - 1] = '\0';
}

// builds command string from my_argv
char *build_cmd(char** my_argv, int *bg) {
    if (!my_argv) {
        return NULL;
    }

    size_t total_args = 0;
    size_t total_size = 0;
    for (int i = 0; my_argv[i]; i++) {
        total_args++;
        total_size += strlen(my_argv[i]) + 1;
    }

    char *built_cmd = malloc(total_size);
    if (!built_cmd) {
        return NULL;
    }

    char *p = built_cmd;
    for (int i = 0; my_argv[i]; i++) {
        size_t len = strlen(my_argv[i]);
        memcpy(p, my_argv[i], len);
        p += len;
        if (my_argv[i + 1]) {
            *p = ' ';
            p++;
        } 
    }

    *p = '\0';
    // background setup
    if (*(p - 1) == '&') {
        *bg = 1;
        my_argv[total_args - 1] = NULL;
    }

    return built_cmd;
}

// executes input argument array
void execute(char **my_argv) {
    int background = 0;
    pid_t child_pid;
    char *cmd = build_cmd(my_argv, &background);

    // handle builtin commands and null argument
    if (my_argv[0] == NULL) {
        return;
    } else if (builtin_cmd(my_argv)) {
        return;
    }

    int jid = get_new_jid();
    if (jid == -1) {
        fprintf(stderr, "shell jobs array is full");
        return;
    }

    // block SIGCHLD and job control signals before fork so the child
    // inherits a blocked mask — prevents inherited handlers from catching
    // signals before we can reset them to SIG_DFL in the child
    sigset_t mask_fork, prev_mask;
    sigemptyset(&mask_fork);
    sigaddset(&mask_fork, SIGCHLD);
    sigaddset(&mask_fork, SIGTSTP);
    sigaddset(&mask_fork, SIGINT);
    sigaddset(&mask_fork, SIGQUIT);
    sigprocmask(SIG_BLOCK, &mask_fork, &prev_mask);

    child_pid = fork();

    // create child process
    if (child_pid == 0) {
        setpgid(0, 0);
        // reset signal handlers to default before executing child process
        signal(SIGINT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
        signal(SIGCHLD, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);
        // unblock all signals for the child
        sigset_t empty;
        sigemptyset(&empty);
        sigprocmask(SIG_SETMASK, &empty, NULL);
        execvp(my_argv[0], my_argv);
        fprintf(stderr, "execvp\n");
        exit(1);
    }

    setpgid(child_pid, child_pid);
    add_job(child_pid, jid, cmd, background);
    // unblock SIGCHLD in child process, reverts signal mask to previous state. we don't need to save current mask
    sigprocmask(SIG_SETMASK, &prev_mask, NULL);
    free(cmd);

    // repeat shell loop if background job or wait if foreground job
    if (background) {
        jobs[jid - 1].state = BG;
        printf("[bg] started pid %d jid %d (%s)\n", child_pid, jid, jobs[jid - 1].cmdline);
    } else {
        jobs[jid - 1].state = FG;
        // give terminal control to child process group
        tcsetpgrp(STDIN_FILENO, child_pid);
        printf("[fg] started pid %d jid %d (%s)\n", child_pid, jid, jobs[jid - 1].cmdline);
        sigset_t empty;
        sigemptyset(&empty);
        while (child_pid == get_fgpid()) {
            sigsuspend(&empty);
        }
        // reclaim terminal control for the shell
        tcsetpgrp(STDIN_FILENO, getpgrp());
    }
}
