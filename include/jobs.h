#pragma once
#include <common.h>

pid_t get_fgpid(void);
int get_new_jid(void);
void reset_job(int jid);
int get_jid(int id);
void print_done_jobs(void);
