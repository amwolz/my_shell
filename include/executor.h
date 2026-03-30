#pragma once
#include <common.h>
#include <signals.h>

void make_bgfg(char **my_argv, int bgfg);
int builtin_cmd(char **my_argv);
void print_help(void);
void ls(void);
void echo(char **my_argv);
void pwd(void);
void add_job(pid_t pid, int jid, char *cmd, int background);
char *build_cmd(char** my_argv, int *bg);
void execute(char **my_argv);
