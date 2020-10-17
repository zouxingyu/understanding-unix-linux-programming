#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#define USE_DUP2
int main(int argc, char *argv[])
{
    char buf[100];
    fgets(buf, 100, stdin);
    printf("%s", buf);
    fgets(buf, 100, stdin);
    printf("%s", buf);
    fgets(buf, 100, stdin);
    printf("%s", buf);

    int fd, newfd;
    fd = open("/etc/passwd", O_RDONLY);
#ifndef USE_DUP2
    close(0);
    newfd = dup(fd);
#else
    newfd = dup2(fd, 0);
#endif
    if(newfd == -1){
        perror("dup");
        exit(1);
    }
    close(fd);

    fgets(buf, 100, stdin);
    printf("%s", buf);
    fgets(buf, 100, stdin);
    printf("%s", buf);
    fgets(buf, 100, stdin);
    printf("%s", buf);
    return 0;
}
