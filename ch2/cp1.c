#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#define MAXSIZE 128

char *GetNewPath(char *newPath, char *source, char *destination) {
    int sourceLen = strlen(source);
    int desLen = strlen(destination);
    int i;
    for (i = sourceLen; i >= 0; --i) {
        if (source[i] == '/') {
            break;
        }
    }
    strcat(newPath, destination);
    newPath[desLen] = '/';
    newPath[desLen + 1] = '\0';
    strcat(newPath, source + i + 1);
    return newPath;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "usage: %s source destination", *argv);
        exit(1);
    }
    char *source, *destination;
    if (argc == 3) {
        source = argv[1];
        destination = argv[2];
    } else if (argc == 4 && !strcmp(argv[1], "-i")) {
        source = argv[2];
        destination = argv[3];
    } else {
        fprintf(stderr, "parse error");
        exit(1);
    }
    struct stat stInput, stOutput;
    if (stat(source, &stInput) == -1) {
        perror("stat source failed");
        exit(1);
    }
    bool exist = true;
    bool isDestiDir = false;
    if(stat(destination, &stOutput) == -1){
        perror("stat destination failed");
        exit(1);
    }
    if (S_ISDIR(stOutput.st_mode)) {
        isDestiDir = true;
    }
    if (isDestiDir) {
        char newPath[MAXSIZE];
        destination = GetNewPath(newPath, source, destination);
    }
    if (stat(destination, &stOutput) == -1) {
        if (errno == ENOENT) {
            exist = false;
        } else {
            perror("stat destination failed");
            exit(1);
        }
    }
    if (exist) {
        if (stInput.st_dev == stOutput.st_dev &&
            stOutput.st_ino == stInput.st_ino) {
            fprintf(stderr, "%s and %s refer to the same file", source,
                    destination);
            exit(1);
        }
    }
    int fdInput, fdOutput;
    if ((fdInput = open(source, O_RDONLY)) == -1) {
        perror("open source failed");
        exit(1);
    }
    if (!exist) {
         if ((fdOutput = open(destination, O_CREAT | O_WRONLY)) == -1) {
            perror("open destination failed");
            exit(1);
        }
    } else {
        if (argc == 4) {
            printf("destination file alreay exist, do you want to proceed?");
            FILE *ptr = fopen("/dev/pts/0", "r");
            char c;
            if ((c = getc(ptr)) != EOF) {
                if (c != 'y') exit(0);
            }
        }
        if ((fdOutput = open(destination, O_TRUNC | O_WRONLY)) == -1) {
            perror("open destination failed");
            exit(1);
        }
        
    }
    char buf[MAXSIZE];
    int nByte;
    while ((nByte = read(fdInput, buf, MAXSIZE)) > 0) {
        if (write(fdOutput, buf, nByte) == -1) {
            perror("write failed");
            exit(1);
        }
    }
    if (nByte == -1) {
        perror("read failed");
        exit(1);
    }
    return 0;
}
