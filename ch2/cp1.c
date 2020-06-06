#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define BUFFERSIZE 4096
#define COPYMODE 0644

void oops(char *, char *);
int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "usage: %s source destination\n", *argv);
        exit(1);
    }
    int in_fd, out_fd, n_chars;
    char buf[BUFFERSIZE];

    if ((in_fd = open(argv[1], O_RDONLY)) == -1)
        oops("cannot open", argv[1]);
    if ((out_fd = creat(argv[2], COPYMODE)) == -1)
        oops("cannot creat", argv[2]);
    while ((n_chars = read(in_fd, buf, BUFFERSIZE)) > 0) {
        if (write(out_fd, buf, n_chars) != n_chars)
            oops("write error to", argv[2]);
    }
    if (n_chars == -1)
        oops("read error from", argv[1]);
    if (close(in_fd) == -1 || close(out_fd) == -1)
        oops("error closing file", "");
}
void oops(char *s1, char *s2) {
    fprintf(stderr, "error:%s", s1);
    perror(s2);
    exit(1);
}