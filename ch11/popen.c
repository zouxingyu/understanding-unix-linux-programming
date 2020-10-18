#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#define BUFSIZE 100
#define FUNCERROR(str)                \
    do {                              \
        fprintf(stderr, "%s\n", str); \
        return NULL;                  \
    } while (0)
#define SYSERROR(str) \
    do {              \
        perror(str);  \
        exit(1);      \
    } while (0);
FILE *POpen(char *command, char *mode) {
    if (strlen(mode) != 1) FUNCERROR("invalid mode");
    int flag;
    char c = *mode;
    if (c == 'w') {
        flag = 0;
    } else if (c == 'r') {
        flag = 1;
    } else {
        FUNCERROR("invalid mode");
    }
    int array[2];
    if (pipe(array) == -1)FUNCERROR("pipe error");
    int pid = fork();
    if (pid == -1) {
        FUNCERROR("fork error");
    } else if (!pid) {
        if (flag == 0) {
            close(array[1]);
            if (dup2(array[0], 0) == -1) SYSERROR("dup2");
            close(array[0]);
            execlp("sh", "sh", "-c", command, (char *)NULL);
            SYSERROR("execlp");
        } else {
            close(array[0]);
            if (dup2(array[1], 1) == -1) SYSERROR("dup2");
            close(array[0]);
            execlp("sh", "sh", "-c", command, (char *)NULL);
            SYSERROR("execlp");
        }
    } else {
        if (flag == 0) {
            close(array[0]);
            return fdopen(array[1], mode);
        } else {
            close(array[1]);
            return fdopen(array[0], mode);
        }
    }
}
void PClose(FILE *p) {
    wait(NULL);
    fclose(p);
}
int main(int argc, char *argv[]) {
    char buf[BUFSIZ];
    FILE *p = POpen("ps|sort", "r");
    if(!p) SYSERROR("POpen");
    printf("-----------------\n");
    while (fgets(buf, BUFSIZE, p)) {
        printf("%s", buf);
    }
    if (!p) SYSERROR("POpen");
    PClose(p);
    return 0;
}
