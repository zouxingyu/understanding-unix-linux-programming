#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
char *str = "testing";
int main() {
    int fd = open("file", O_RDWR | O_TRUNC);
    int pos;
    // if ((pos = lseek(fd, 20, SEEK_SET)) == -1) {
    //     perror("seek failed");
    //     exit(1);
    // }
    // char buf[128];
    //read from the file after lseek past the end of file does not change the file size
    //print whatever in pos
    //write from the file after lseek past the end of file will increase file size
    // if (read(fd, buf, 10) == -1) {
    //     perror("reada failed");
    //     exit(1);
    // }
    if (write(fd, str, strlen(str)) == -1) {
        perror("write failde");
    }
    // printf("%s", buf);
}