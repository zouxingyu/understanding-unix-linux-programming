#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#define MAXSIZE 1024
#define CHUNCKSIZE 64
char buf[CHUNCKSIZE];
void ErrorHandler(char *str) {
    perror(str);
    exit(0);
}
int ReadBytes(int fd, off_t *ofPtr) {
    if (*ofPtr == 0) return -1;
    if (*ofPtr < CHUNCKSIZE) {
        int readSize = *ofPtr;
        *ofPtr = lseek(fd, 0, SEEK_CUR);
        if (*ofPtr == -1) ErrorHandler("seek error");
        if (read(fd, buf, readSize) == -1) ErrorHandler("read failed");
        *ofPtr = lseek(fd, -readSize, SEEK_CUR);
        if (*ofPtr == -1) ErrorHandler("seek error");
        return readSize;
    } else {
        *ofPtr = lseek(fd, -CHUNCKSIZE, SEEK_CUR);
        if (*ofPtr == -1) ErrorHandler("seek error");
        if (read(fd, buf, CHUNCKSIZE) == -1) ErrorHandler("read failed");
        *ofPtr = lseek(fd, -CHUNCKSIZE, SEEK_CUR);
        if (*ofPtr == -1) ErrorHandler("seek error");
        return CHUNCKSIZE;
    }
}
int main(int argc, char *argv[]) {
    int fd;
    int lineNum = 11;
    if (argc == 2) {
        fd = open(argv[1], O_RDONLY);
        if (fd == -1) ErrorHandler("open failed");

    } else if (argc == 4 && !strcmp(argv[1], "-n")) {
        lineNum = atoi(argv[2])+1;
         fd = open(argv[3], O_RDONLY);
        if (fd == -1) ErrorHandler("open failed");
    } else {
        fprintf(stderr, "parse error");
        exit(0);
    }
    int nbyte;
    int lines = 0;
    int total = 0;
    off_t of = lseek(fd, 0, SEEK_END);
    if (of == -1) ErrorHandler("seek error");
    while ((nbyte = ReadBytes(fd, &of)) != -1) {
        for (--nbyte; nbyte >= 0; --nbyte) {
            if (buf[nbyte] == '\n') {
                if (++lines >= lineNum) break;
            }
            ++total;
        }
        if (lines >= lineNum) break;
    }
    char tmp[MAXSIZE];
    lseek(fd, -total, SEEK_END);
    read(fd, tmp, total);
    write(1, tmp, total);
    return 0;
}
