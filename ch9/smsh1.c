#include <ctype.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#define PROMPT "$"
#define BUFFERSIZE 32
#define ARGSIZE 32
void *Emalloc(size_t size) {
    void *ptr = malloc(size);
    if (ptr == NULL) {
        fprintf(stderr, "malloc failed");
        exit(1);
    }
    return ptr;
}
void *Erealloc(void *ptr, size_t size) {
    void *nptr = realloc(ptr, size);
    if (ptr == NULL) {
        fprintf(stderr, "realloc failed");
        exit(1);
    }
    return nptr;
}
char *GetString(char *start, int len) {
    char *ptr = malloc(len + 1);
    memcpy(ptr, start, len);
    ptr[len] = '\0';
    return ptr;
}
char *GetCommand(FILE *input, int *lenPtr) {
    int c;
    int len = 0;
    int bufSize = 0;
    char *buf;
    printf("%s", PROMPT);
    while ((c = getc(input)) != EOF) {
        if (!bufSize) {
            buf = Emalloc(BUFFERSIZE);
            bufSize = BUFFERSIZE;
        } else if (len + 1 >= bufSize) {
            buf = Erealloc(buf, bufSize + BUFFERSIZE);
            bufSize += BUFFERSIZE;
        }
        buf[len++] = c;
        if (c == '\n') break;
    }
    if (c == EOF) return NULL;
    buf[len] = '\0';
    *lenPtr = len;
    return buf;
}
char **GetArgList(char *command, int commandLen) {
    if (!command || *command == '\0') {
        return NULL;
    }
    char **argList;
    int argListSize = 0;
    int argListPos = 0;
    char *start = command;
    char *end = command + commandLen;
    // printf("%s, %d\n", command, commandLen);
    // remvoe leading and trailing whitespace
    if (start < end && isspace(*start)) ++start;
    if (start == end) {
       // fprintf(stderr, "empty line");
        return NULL;
    }
    bool inSpace = true;
    int len;
    char *pos = start;
    int spots = 0;
    while (start < end) {
        if (isspace(*start)) {
            if (!inSpace) {
                inSpace = true;
                if (!argListPos) {
                    argList = Emalloc(ARGSIZE);
                    spots += ARGSIZE / sizeof(char *);
                    argListSize += ARGSIZE;
                } else if (argListPos + 1 >= spots) {
                    argList = Erealloc(argList, argListSize + ARGSIZE);
                    spots += ARGSIZE / sizeof(char *);
                    argListSize += ARGSIZE;
                }
                argList[argListPos++] = GetString(pos, len);
                //        printf("\n%d, %s\n", argListPos - 1,
                //        argList[argListPos - 1]);
            }
        } else if (inSpace) {
            len = 1;
            inSpace = false;
            pos = start;
        } else {
            ++len;
        }
        ++start;
    }
    argList[argListPos] = NULL;
    return argList;
}
int Execute(char *argList[]) {
    int pid;
    int wstatus;
    int ret;
    pid = fork();
    if (!pid) {
        signal(SIGINT, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);
        execvp(argList[0], argList);
        perror("execvp");
        exit(1);
    } else if (pid != -1) {
        ret = wait(&wstatus);
        if (ret == -1) {
            perror("wait");
            return -1;
        } else {
            return wstatus;
        }
    } else {
        perror("fork");
        return -1;
    }
}
void FreeArgList(char *argList[]) {
    if(argList){
        char **ptr = argList;
        while (*ptr) {
            free(*ptr++);
        }
        free(argList);
    }
}
void PrintResult(int result) {
    if (WIFEXITED(result)) {
        printf("terminated normally, exit status:%d\n", WEXITSTATUS(result));
    } else if (WIFSIGNALED(result)) {
        printf("terminated by signal, signum:%d\n", WTERMSIG(result));
    }
}
int main(int argc, char *argv[]) {
    setbuf(stdout, NULL);
    char *command;
    char **argList;
    int result;
    int len;
    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    while (command = GetCommand(stdin, &len)) {
        if (argList = GetArgList(command, len)) {
            result = Execute(argList);
            if (result == -1) {
                fprintf(stderr, "Execute failed\n");
            } else {
                PrintResult(result);
            }
        }
        FreeArgList(argList);
    }
    free(command);
    return 0;
}
