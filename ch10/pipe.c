#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

void Oops(char *str){
    perror(str);
    exit(1);
}
int main(int argc, char *argv[])
{
    if(argc != 3){
        fprintf(stderr, "usage: pipe cmd1 cmd2\n");
        exit(1);
    } 
    int array[2];
    if(pipe(array) == -1)Oops("pipe");
    int pid = fork();
    if(pid == -1){
        Oops("fork");
    }else if(!pid){
        close(array[1]);
        if(dup2(array[0],0) == -1)Oops("dup2");
        close(array[0]);
        execlp(argv[2], argv[2], (char*)NULL);
        Oops("execlp");
    }else {
        close(array[0]);
        if(dup2(array[1], 1) == -1)Oops("dup2");
        close(array[0]);
        execlp(argv[1], argv[1], (char *)NULL);
        Oops("execlp");
    }
    return 0;
}
