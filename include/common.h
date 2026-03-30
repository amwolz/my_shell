#pragma once
#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <dirent.h>
#include <errno.h>
#include <linux/limits.h>
#include <sys/prctl.h>

#define MAX_LINE 1024
#define MAX_ARGS 64
#define MAX_JOBS 16

#define UNDEF 0
#define FG 1
#define BG 2
#define PST 3
#define K 4
#define TE 5
#define DONE 6

typedef struct {
    pid_t pid;
    int jid;
    int state;
    char cmdline[MAX_LINE];
} job_t;

extern job_t jobs[MAX_JOBS];
