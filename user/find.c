#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void 
find(char *path, char *targetFile) 
{
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if ((fd = open(path, 0)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    if (st.type != T_DIR) {
        fprintf(2, "Usage: find <DIRECTORY> <filename>\n");
        close(fd);
        return;
    }

    if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
        fprintf(2, "find: path too long\n");
        close(fd);
        return;
    }

    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';
    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
        if(de.inum == 0) continue;
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;
        if (stat(buf, &st) < 0) {
            printf("find: cannot stat %s\n", buf);
            continue;   // skip
        }

        switch(st.type) {
            case T_FILE:
                if (strcmp(targetFile, p) == 0) {
                    printf("%s\n", buf);
                }
                break;
            case T_DIR:
                if (strcmp(".", p) != 0 && strcmp("..", p) != 0) {
                    find(buf, targetFile);
                }
        }
    }
}

int  
main(int argc, char *argv[]) 
{
    if (argc != 3) {
        fprintf(2, "Usage: find <DIRECTORY> <filename>\n");
        exit(1);
    }

    find(argv[1], argv[2]);
    exit(0);
}