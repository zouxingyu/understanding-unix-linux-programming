#include <bits/types/siginfo_t.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#define MAXARGS 20
#define ARGLEN 100
#define EXIT "exit"
char *Trim(char *argBuf){
    int len = strlen(argBuf);
    argBuf[len - 1] = '\0';
    char *ptr = malloc(len);
    return memcpy(ptr, argBuf, len + 1);
}
void DeleteArgList(char *argList[], int argNums){
    for(int i = 0; i < argNums; ++i){
        free(argList[i]);
    }
}   
int main(int argc, char *argv[])
{
    char *argList[MAXARGS];
    char argBuf[ARGLEN];

    int argNums = 0;
    while(argNums < MAXARGS){
        printf("Arg[%d]", argNums);
        if(fgets(argBuf, ARGLEN, stdin) && *argBuf != '\n'){
            argList[argNums] = Trim(argBuf);
            if(!argNums && !strcmp(argList[0], EXIT)) exit(1);
            argNums++;
        }else{
            if(feof(stdin))exit(1);
            argList[argNums] = NULL;
            int wstatus;
            int pid = fork();
            if(pid == -1){
                perror("fork");
                DeleteArgList(argList, argNums);
                exit(1);
            }else if(!pid){
                execvp(argList[0], argList);
                DeleteArgList(argList, argNums);
                perror("execvp");
                exit(1);
            }else{
                while(wait(&wstatus) != pid)
                    ;
                printf("child exit with status [ret:%d, sig:%d, core:%d]\n", wstatus>>8, wstatus&0x7F, wstatus&0x80);
            }
            argNums = 0;
        }
    }
    return 0;
}
