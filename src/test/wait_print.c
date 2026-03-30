#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char **argv) {

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <n>\n", argv[0]);
        exit(0);
    }

    float i = 0;
    float time = atof(argv[1]);

    for (i=0; i < time; i++) {
        sleep(1);
        printf("test %f\n", i);
    }
    
    exit(0);
}