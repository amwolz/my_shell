#include <builtins.h>

// brings a stopped job into the background or foreground
void make_bgfg(char **my_argv, int bgfg) {
    if (my_argv[1] != NULL && my_argv[1][0] == '%') {
        int id = atoi(my_argv[1] + 1);
        int jid = get_jid(id);
        if (jid < 0) {
            fprintf(stderr, "%s %%%d: no such job\n", my_argv[0], id);
            return;
        }
        jobs[jid - 1].state = bgfg;
        kill(-jobs[jid - 1].pid, SIGCONT);
        pid_t pid = jobs[jid - 1].pid;
        // emits new prompt or waits
        if (bgfg == BG) {
            printf("[bg] continued pid %d jid %d\n", pid, get_jid(pid));
        } else {
            // give terminal control to the resumed process group
            tcsetpgrp(STDIN_FILENO, pid);
            printf("[fg] continued pid %d jid %d\n", pid, get_jid(pid));
            sigset_t empty;
            sigemptyset(&empty);
            while (pid == get_fgpid()) {
                sigsuspend(&empty);
            }
            // reclaim terminal control for the shell
            tcsetpgrp(STDIN_FILENO, getpgrp());
            sigprocmask(SIG_SETMASK, &empty, NULL);
        }
    } else {
        fprintf(stderr, "bg/fg: usage bg/fg %%<jid>\n");
    }
}

// checks if command is a builtin command
int builtin_cmd(char **my_argv) {
    if (strcmp(my_argv[0], "quit") == 0 || strcmp(my_argv[0], "q") == 0) {
        exit(0);
    } else if (strcmp(my_argv[0], "h") == 0 || strcmp(my_argv[0], "help") == 0) {
        print_help();
        return 1;
    } else if (strcmp(my_argv[0], "ls") == 0) {
        ls();
        return 1;
    } else if (strcmp(my_argv[0], "echo") == 0) {
        echo(my_argv);
        return 1;
    } else if (strcmp(my_argv[0], "pwd") == 0) {
        pwd();
        return 1;
    } else if (strcmp(my_argv[0], "lj") == 0 || strcmp(my_argv[0], "listjobs") == 0) {
        listjobs();
        return 1;
    } else if (strcmp(my_argv[0], "cd") == 0) {
        if (my_argv[1] == NULL) {
            fprintf(stderr, "cd: missing argument\n");
        } else if (chdir(my_argv[1]) != 0) {
            fprintf(stderr, "cd\n");
        }
        return 1;
    } else if (strcmp(my_argv[0], "kill") == 0 || strcmp(my_argv[0], "k") == 0) {
        if (my_argv[1] == NULL || my_argv[1][0] != '%') {
            fprintf(stderr, "kill: usage kill/k %%<jid|pid>\n");
        } else {
            int id = atoi(my_argv[1] + 1);
            int jid = get_jid(id);
            if (jid < 0 || jobs[jid - 1].state == UNDEF || jobs[jid - 1].state == TE || jobs[jid - 1].state == DONE) {
                fprintf(stderr, "kill %%%d: no such job\n", id);
                return 1;
            } else if (jobs[jid - 1].state == PST) {
                printf("\nJob ID: [%d] Process ID: (%d) State: Stopped Command: %s\n", jobs[jid-1].jid, jobs[jid-1].pid, jobs[jid-1].cmdline);
            }
            jobs[jid - 1].state = K;
            int pid = jobs[jid - 1].pid;
            kill(-pid, SIGTERM);
        }
        return 1;
    } else if (strcmp(my_argv[0], "stop") == 0 || strcmp(my_argv[0], "s") == 0) {
        if (my_argv[1] == NULL || my_argv[1][0] != '%') {
            fprintf(stderr, "stop: usage stop/s %%<jid|pid>\n");
        } else {
            int id = atoi(my_argv[1] + 1);
            int jid = get_jid(id);
            if (jid < 0 || jobs[jid - 1].state == UNDEF || jobs[jid - 1].state == TE || jobs[jid - 1].state == DONE) {
                fprintf(stderr, "stop %%%d: no such job\n", id);
            } else if (jobs[jid - 1].state == PST) {
                printf("\nJob ID: [%d] Process ID: (%d) State: Stopped Command: %s\n", jobs[jid-1].jid, jobs[jid-1].pid, jobs[jid-1].cmdline);
            } else {
                int pid = jobs[jid - 1].pid;
                kill(-pid, SIGTSTP);
                // wait for SIGCHLD to arrive and update the job state
                sigset_t empty;
                sigemptyset(&empty);
                while (jobs[jid - 1].state == BG) {
                    sigsuspend(&empty);
                }
            }
        }
        return 1;
    } else if (strcmp(my_argv[0], "bg") == 0) {
        make_bgfg(my_argv, BG);
        return 1;
    } else if (strcmp(my_argv[0], "fg") == 0) {
        make_bgfg(my_argv, FG);
        return 1;
    // handles the builtin sleep command as a child process
    // this is convenient for the purpose of this project
    // it allows the user to correctly pause sleep commands
    } else if (strcmp(my_argv[0], "sleep") == 0) {
        my_argv[0] = "./bin/test/wait";
        return 0;
    }
    return 0;
}

// help menu
void print_help(void) {
    printf("%s\n\n", "Help Menu");
    printf("%s\n", "valid builtin commands for my_shell: ");
    printf("%s\n", "help/h: displays help options");
    printf("%s\n", "ls: lists contents in current directory");
    printf("%s\n", "echo: echo prints following command string");
    printf("%s\n", "cd: change current working directory");
    printf("%s\n", "pwd: prints current working directory");
    printf("%s\n", "listjobs/lj: prints the current jobs (can print foreground jobs if interrupted)");
    printf("%s\n", "stop/s %<int job id>: pauses a current job in the jobs array");
    printf("%s\n", "kill/k %<int job id>: terminates a current job in the jobs array");
    printf("%s\n", "bg %<int job id>: runs an interrupted job in the background)");
    printf("%s\n", "fg %<int job id>: runs an interrupted job in the foreground)");
    printf("%s\n\n", "sleep <int n>: delays program for input n amount of seconds");
    printf("%s\n", "interruption options:");
    printf("%s\n", "CTRL-Z: stops (pauses) foreground job");
    printf("%s\n", "CTRL-C: kills foreground job");
    printf("%s\n", "CTRL-D: exits shell");
    printf("%s\n", "CTRL-\\ kills active processes and exits shell");
}

// lists contents in current directory
void ls(void) {
    DIR *dir = opendir(".");
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        printf("%s\n", entry->d_name);
    }
    closedir(dir);
}

// prints following command string
void echo(char **my_argv) {
    int i = 1;
    while (my_argv[i] != NULL) {
        printf("%s", my_argv[i]);
        if (my_argv[i + 1] != NULL)
            printf(" ");
        i++;
    }
    printf("\n");
}

// prints current working directory
void pwd(void) {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        fprintf(stderr, "pwd\n");
        return;
    }
    printf("%s\n", cwd);
}

// prints current jobs array with jid, pid, state and commmand
void listjobs(void) {
    int count = 0;
    for (int i = 0; i < MAX_JOBS; i++) {
        if (jobs[i].pid != 0) {
            count++;
            char *state = "Unknown";
            if (!(jobs[i].state == BG || jobs[i].state == PST || jobs[i].state == TE || jobs[i].state == DONE)) {
                continue;
            } else if (jobs[i].state == BG) {
                state = "Running";
            } else if (jobs[i].state == PST) {
                state = "Stopped";
            } else if (jobs[i].state == TE) {
                state = "Terminated";
            } else if (jobs[i].state == DONE) {
                state = "Done";
            }
            printf("Job ID: [%d] Process ID: (%d) State: %s Command: %s\n", jobs[i].jid, jobs[i].pid, state, jobs[i].cmdline);
            if (jobs[i].state == DONE || jobs[i].state == TE) {
                reset_job(jobs[i].jid);
            }
        }
    }
    if (count == 0) {
        printf("No jobs active\n");
    }
}
