#pragma once
#include <executor.h>

void make_bgfg(char **my_argv, int bgfg);
int builtin_cmd(char **my_argv);
void print_help(void);
void ls(void);
void echo(char **my_argv);
void pwd(void);
void listjobs(void);
