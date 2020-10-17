#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
    int fd;
    char buf[100];
    fgets(buf, 100, stdin);
    printf("%s", buf);
    fgets(buf, 100, stdin);
    printf("%s", buf);
    fgets(buf, 100, stdin);
    printf("%s", buf);
    
    close(0);
    fd = open("/etc/passwd", O_RDONLY);
    if(fd == -1){
        perror("open");
        exit(1);
    }

    fgets(buf, 100, stdin);
    printf("%s", buf);
    fgets(buf, 100, stdin);
    printf("%s", buf);
    fgets(buf, 100, stdin);
    printf("%s", buf);
    return 0;
}
