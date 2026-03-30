#include <jobs.h>

job_t jobs[MAX_JOBS];

// gets the pid of the foreground job
pid_t get_fgpid(void) {
    for (int i = 0; i < MAX_JOBS; i++) {
        if (jobs[i].state == FG) {
            return jobs[i].pid;
        }
    }
    return 0;
}

// gets first available job id for a new job
int get_new_jid(void) {
    for (int i = 0; i < MAX_JOBS; i++) {
        if (jobs[i].state == UNDEF) {
            return i + 1;
        }
    }
    return -1;
}

// resets job so that the slot can be reused
void reset_job(int jid) {
    int idx = jid - 1;
    jobs[idx].pid = 0;
    jobs[idx].state = UNDEF;
    jobs[idx].cmdline[0] = '\0';
}

// gets job id from input job/process id
int get_jid(int id) {
    if (id > 0 && id <= MAX_JOBS && jobs[id - 1].state != UNDEF) {
        if (jobs[id - 1].state == UNDEF || jobs[id - 1].state == TE || jobs[id - 1].state == DONE) {
            return -1;
        }
        return id;
    }
    for (int i = 0; i < MAX_JOBS; i++) {
        if (jobs[i].pid == id) {
            if (jobs[i].state == UNDEF || jobs[i].state == TE || jobs[i].state == DONE) {
                return -1;
            }
            return jobs[i].jid;
        }
    }
    return -1;
}

// prints complete jobs after the execution of the input command
void print_done_jobs(void) {
    for (int i = 0; i < MAX_JOBS; i++) {
        if (jobs[i].state == TE) {
            printf("Job ID: [%d] Process ID: (%d) State: Terminated Command: %s\n", jobs[i].jid, jobs[i].pid, jobs[i].cmdline);
            reset_job(jobs[i].jid);
        } else if (jobs[i].state == DONE) {
            printf("Job ID: [%d] Process ID: (%d) State: Done Command: %s\n", jobs[i].jid, jobs[i].pid, jobs[i].cmdline);
            reset_job(jobs[i].jid);
        } else if (jobs[i].state == K) {
            jobs[i].state = TE;
        }
    }
}
