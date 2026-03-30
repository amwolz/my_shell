#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

// function that mimics a process with several child processes, in this case input amount of separate processes
int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <n>\n", argv[0]);
        exit(0);
    }
    int i = 0;
    int processes = atoi(argv[1]);

    printf("parent started\n");

    // creates a new process for each input amount
    // the child process breaks out of the loop, so parent process is the last to exit
    // the child processes become zombie processes becuase the parent process has still not exited
    for (i=1; i < processes; i++) {
        
        pid_t pid = fork();
        if (pid == 0) {
            if (i == 1) {
                printf("first child started\n");
                sleep(5);
            }
            break;
        } else {
            if (i == 1) {
                sleep(5);
            }
            sleep(1);
        }
    } 

    if (i == processes) {
        printf("parent process finished\n");
    } else {
        printf("process finished\n");
    }

    if (i == 1) {
        printf("first child zombied\n");
    }


    exit(0);
}