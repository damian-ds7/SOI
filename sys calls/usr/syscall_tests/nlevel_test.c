#include <stdlib.h>
#include <stdio.h>
#include <lib.h>

int main(int argc, char *argv[]) {
    int n, fork_count;
    int i;
    message m;
    int max_desc = 0;
    pid_t max_pid;
    
    if(argc < 3 || argc > 4) {
        printf("2 arguments are required: n fork_count [p to print process list]\n");
        return 0;
    }

    n = atoi(argv[1]);
    fork_count = atoi(argv[2]);


    for (i = 0; i < fork_count; ++i) {
        if (fork() == 0) {
            sleep(5);
            return 0;
        }
    }
     
    m.m1_i1 = n;

    _syscall(MM, NLEVELDESC, &m);

    max_desc = m.m1_i1;
    max_pid = m.m1_i2;

    if (argc == 4 && *argv[3] == 'p')
        system("ps -xl");

    printf("Max descendants: %d\n", max_desc);
    printf("PID with the most descendants: %d\n", max_pid);

    return 0;
}
