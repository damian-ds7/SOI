#include <stdio.h>
#include <lib.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>

int main(int argc, char *argv[]) {
    char group;
    message m;
    
    if (argc != 2) {
        printf("One argument required either b or c to set process group\n");
        return 0;
    }

    group = toupper(argv[1][0]);

    if (group != 'B' && group != 'C') {
        printf("Wrong argument given, must be either b or c\n");
	return 0;
    }

    m.m1_i1 = group;
    m.m1_i2 = getpid();
    
    if (_syscall(MM, SETGROUP, &m) == -1) {
        printf("Error setting group\n");
        return 0;
    }

    while (1) {}
}
