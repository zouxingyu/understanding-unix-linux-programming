#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
void do_ls(char *);
void unix_error(char *strerror);
int main(int argc, char **argv) {
    if (argc == 1) {
        do_ls(".");
    } else {
        while (--argc) {
            printf("%s:\n", *++argv);
            do_ls(*argv);
        }
    }
}
void do_ls(char *dirname) {
    DIR *dirp;
    struct dirent *dirent;
    if ((dirp = opendir(dirname)) == NULL) {
        unix_error("opendir error");
    }
    while ((dirent = readdir(dirp)) != NULL) {
        printf("%s\n", dirent->d_name);
    }
    closedir(dirp);
}
void unix_error(char *strerror) {
    perror(strerror);
    exit(1);
}