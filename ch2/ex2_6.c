#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
char *str = "testing 1 2 3 ...";
int main() {
    char buf[128];
    int fd1, fd2, fd3;
    if ((fd1 = open("file", O_RDONLY)) == -1) {
        perror("cannot open");
    }
    if ((fd2 = open("file", O_WRONLY)) == -1) {
        perror("cannot open");
    }
    if ((fd3 = open("file", O_RDONLY)) == -1) {
        perror("cannot open");
    }
    int nchars;
    if ((nchars = read(fd1, buf, 20)) != -1) {
        printf("%s\n", buf);
    } else {
        perror("cannot read");
    }
    if (write(fd2, str, strlen(str)) == -1) {
        perror("cannot write");
    }
    if ((nchars = read(fd3, buf, 20)) != -1) {
        printf("%s\n", buf);
    } else {
        perror("cannot read");
    }
}