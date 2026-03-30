#include <common.h>
#include <jobs.h>
#include <readline/readline.h>

// terminates all child processes of input process (pgid is not an option)
void terminate_from_process(pid_t parent_pid) {
    DIR *proc = opendir("/proc");
    if (proc) {
        struct dirent *ent;
        while ((ent = readdir(proc)) != NULL) {
            // skip non-numeric entries (non-PID directories)
            pid_t child_pid = atoi(ent->d_name);
            if (child_pid <= 0) {
                continue;
            }

            // read /proc/<pid>/stat to get ppid
            char path[64];
            snprintf(path, sizeof(path), "/proc/%d/stat", child_pid);
            FILE *f = fopen(path, "r");
            if (!f) continue;

            int pid;
            char comm[256];
            char state;
            int ppid;
            if (fscanf(f, "%d %255s %c %d", &pid, comm, &state, &ppid) == 4) {
                if (ppid == parent_pid) {
                    kill(pid, SIGTERM);
                }
            }
            fclose(f);
        }
        closedir(proc);
    }
}

// handles CTRL-C
void sigint_handler(int signo) {
    (void)signo;
    write(STDOUT_FILENO, "\n", 1);
    rl_on_new_line();
    rl_replace_line("", 0);
    rl_redisplay();
}

// handles CTRL-Z
void sigtstp_handler(int signo) {
    (void)signo;
    if (!get_fgpid()) {
        rl_on_new_line();
        rl_redisplay();
        write(STDOUT_FILENO, "\033[K", 3);
    }
    
}

// handles quit signals (CTRL-\)
void sigquit_handler(int signo) {
    (void)signo;
    printf("Terminating after receipt of quit signal\n");
    kill(-get_fgpid(), SIGTERM);
    signal(SIGQUIT, SIG_DFL);
    raise(SIGQUIT);
}

// handles SIGCHLD signal (propagated with a state change to a child process)
void sigchld_handler(int signo) {
    (void)signo;
    int status;
    pid_t pid;
    // reap zombie processes and detect stopped children (WUNTRACED)
    while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED)) > 0) {
        int jid = get_jid(pid);
        if (jid > 0) {
            // stopped job
            if (WIFSTOPPED(status)) {
                if (jobs[jid - 1].state == FG || jobs[jid - 1].state == BG || jobs[jid - 1].state == PST) {
                    printf("\nJob ID: [%d] Process ID: (%d) State: Stopped Command: %s\n", jobs[jid-1].jid, jobs[jid-1].pid, jobs[jid-1].cmdline);
                    jobs[jid - 1].state = PST;
                }
            // manually terminated job
            } else if (WIFSIGNALED(status)) {
                if (jobs[jid - 1].state == FG) {
                    printf("\n");
                    reset_job(jid);
                } else {
                    jobs[jid - 1].state = TE;
                }
            // regularly exited foreground job
            } else if (jobs[jid - 1].state == FG) {
                reset_job(jid);
            // regularly exited background job
            } else {
                jobs[jid - 1].state = DONE;
            }
        }
    }
}

// sets up signal listeners
void setup_signals(void) {
    struct sigaction sa = {0};
    sigemptyset(&sa.sa_mask);

    // SIGCHLD needs SA_RESTART so waitpid-based loops aren't interrupted
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = sigchld_handler;
    sigaction(SIGCHLD, &sa, NULL);

    // SIGINT without SA_RESTART so fgets returns on CTRL-C (allows re-prompting)
    sa.sa_flags = 0;
    sa.sa_handler = sigint_handler;
    sigaction(SIGINT, &sa, NULL);

    // SIGTSTP and SIGQUIT with SA_RESTART
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = sigtstp_handler;
    sigaction(SIGTSTP, &sa, NULL);

    sa.sa_handler = sigquit_handler;
    sigaction(SIGQUIT, &sa, NULL);

    // ignore SIGTTOU so shell can call tcsetpgrp from background
    signal(SIGTTOU, SIG_IGN);
}
