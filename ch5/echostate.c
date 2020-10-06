#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    struct termios info;
    int rv = tcgetattr(0, &info);
    if (rv == -1) {
        perror("tcgetattr");
        exit(1);
    }
    if (info.c_cflag & ECHO)
        printf("echo is on\n");
    else
        printf("echo is off\n");
    return 0;
}
