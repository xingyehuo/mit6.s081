#include "kernel/types.h"
#include "user/user.h"
#include "kernel/param.h"

#define MAX_LEN 100

int 
main (int argc, char* argv[])
{
    char* command = argv[1];
    char ch;    // store charactor from stdin
    char paramv[MAXARG][MAX_LEN];
    char *curParam[MAXARG];

    while(1){   // loop process parameters for each line in stdin
        int count = argc - 1;
        memset(paramv, 0, MAXARG * MAX_LEN);

        for (int i = 1; i < argc; i++){ 
            strcpy(paramv[i-1], argv[i]);
        }

        int read_res = 0; // store the result of read
        int flag = 0;   // record whether there is parameter before space
        int cursor = 0; // each parameter's index

        while ((read_res = read(0, &ch, 1)) > 0 && ch != '\n'){
            if(ch == ' ' && flag == 1) {
                count++;
                cursor = 0;
                flag = 0;
            } else if(ch != ' '){
                paramv[count][cursor++] = ch;
                flag = 1;
            }
        }

        if (read_res <= 0){
            break;
        }

        for (int i = 0; i < MAXARG - 1; i++) {
            curParam[i] = paramv[i];
        }
        curParam[MAXARG - 1] = '\0';

        if (fork() == 0){
            exec(command, curParam);
        } else {
            wait((int *)0);
        }
    }
    exit(0);
} 