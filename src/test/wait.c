#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

// delays for input amount of seconds, mimics sleep
int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <n>\n", argv[0]);
        exit(0);
    }
    float time = atof(argv[1]);
    sleep(time);
    exit(0);
}
