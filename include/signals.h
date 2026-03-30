#pragma once

#include <common.h>
#include <jobs.h>

void terminate_from_process(pid_t parent_pid);
void sigint_handler(int signo);
void sigtstp_handler(int signo);
void sigquit_handler(int signo);
void sigchld_handler(int signo);
void setup_signals(void);
