#include <stdlib.h>
#include <stdio.h>
#include <lib.h>

int main(int argc, char *argv[]) {
    int a, b, fork_count;
    int i;
    message m;
    int max_desc = 0;
    pid_t max_pid;

    if(argc < 4 || argc > 5) {
        printf("3 arguments are required: a b fork_count [p to print process info]\n");
        return 0;
    }

    a = atoi(argv[1]);
    b = atoi(argv[2]);
    fork_count = atoi(argv[3]);

    for (i = 0; i < fork_count; ++i) {
        if (fork() == 0) {
            sleep(5);
            return 0;
        }
    }
    
    m.m1_i1 = a;
    m.m1_i2 = b;

    _syscall(MM, MAXDESC, &m);

    max_desc = m.m1_i1;
    max_pid = m.m1_i2;

    if (max_desc == 0) {
        printf("Unable to find a process with number of descendants in given range\n");
    } else {
        if (argc == 5 && *argv[4] == 'p')
            system("ps -xl");

        printf("Max descendants: %d\n", max_desc);
        printf("PID with the most descendants: %d\n", max_pid);
    }

    return 0;
}
