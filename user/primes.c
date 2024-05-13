#include "kernel/types.h"
#include "user/user.h"

#define RD 0	// pipe's read port
#define WR 1	// pipe's write port

void child(int *parentPipe);    // recurive function to process child process

int 
main(int argc, char *argv[]) 
{
    int mainPipe[2];   // write 2~35 to pipe
    pipe(mainPipe);
    int pid = fork();

    if (pid == 0) {
        // child process
        child(mainPipe);
    } else {
        close(mainPipe[RD]);
        for (int i = 2; i <= 35; i++) {
            write(mainPipe[WR], &i, sizeof(int));
        }
        close(mainPipe[WR]);
        wait((int *)0); // waiting for the child process to exit
    }

    exit(0);
}

void child(int *parentPipe)
{
    int n;
    int curPipe[2];

    close(parentPipe[WR]);
    int read_num = read(parentPipe[RD], &n, sizeof(int));  // read first num from parent's pipe, and it must be a prime
    if (read_num == 0) {
        // no num in parent's pipe, end recursion
        close(parentPipe[RD]);
        exit(0);
    }

    pipe(curPipe);
    int pid = fork();

    if (pid == 0) {
        // child process, entry recursion
        child(curPipe);
    } else {
        close(curPipe[RD]);
        printf("prime %d\n", n);

        int prime = n;

        while (read(parentPipe[RD], &n, sizeof(int))) {
            if (n % prime != 0) {
                write(curPipe[WR], &n, sizeof(int));
            }
        }
    }
    close(parentPipe[RD]);
    close(curPipe[WR]);

    wait((int *) 0);
    exit(0);
}