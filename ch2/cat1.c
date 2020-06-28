#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAXBUF 4096
char buf[MAXBUF];
void unix_error(char *);
int main(int argc, char **argv) {
    if (argc == 1) {
        fprintf(stderr, "usege: %s sourse file", *argv);
        exit(0);
    }
    FILE *ptr;
    if ((ptr = fopen(argv[1], "r")) == NULL) {
        unix_error(argv[1]);
    }
    while (fgets(buf, MAXBUF, ptr) != NULL) {
        printf("%s", buf);
    }
    fclose(ptr);
}
void unix_error(char *str) {
    perror(str);
    exit(1);
}