#include "kernel/types.h"
#include "user/user.h"

#define RD 0
#define WR 1

int 
main(int argc, const char *argv[])
{
    int ptoc[2];    // parent->child
    int ctop[2];    // child->parent

    pipe(ptoc);
    pipe(ctop);

    int pid = fork();
    char buf = 'A'; // Byte used to transmit

    if (pid < 0) {
        close(ptoc[0]);
        close(ptoc[1]);
        close(ctop[0]);
        close(ctop[1]);
        fprintf(2, "fork failed\n");
        exit(1);
    } else if (pid == 0) {
        // child process
        close(ctop[0]); // close parent read from child
        close(ptoc[1]); // close parent write to child
        
        // child read from parent
        if (read(ptoc[RD], &buf, sizeof(char)) != sizeof(char)) {
            close(ctop[1]);
            close(ptoc[0]);
            fprintf(2, "child read failed\n");
            exit(1);
        } else {
            printf("%d: received ping\n", getpid());
        }

        // child write to parent
        if (write(ctop[WR], &buf, sizeof(char)) != sizeof(char)) {
            close(ctop[1]);
            close(ptoc[0]);
            fprintf(2, "child write failed\n");
            exit(1);
        }

        close(ctop[1]);
        close(ptoc[0]);

        exit(0);
    } else {
        // parent process
        close(ctop[1]); // close child write to parent
        close(ptoc[0]); // close child read from parent
        
        // parent write to child
        if (write(ptoc[WR], &buf, sizeof(char)) != sizeof(char)) {
            close(ctop[0]);
            close(ptoc[1]);
            fprintf(2, "parent write failed\n");
            exit(1);
        }

        // parent read from child
        if (read(ctop[RD], &buf, sizeof(char)) != sizeof(char)) {
            close(ctop[0]);
            close(ptoc[1]);
            fprintf(2, "parent read failed\n");
            exit(1);
        } else {
            printf("%d: received pong\n", getpid());
        } 

        close(ctop[0]);
        close(ptoc[1]);

        exit(0);  
    }
}