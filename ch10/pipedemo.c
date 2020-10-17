#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#define BUFSIZE 100
#define CHILD_MSG "write by child ..."
#define PAR_MSG "write by parent ..."

char buf[BUFSIZ];
int ary[2];

void Oops(char *str){
    perror(str);
    exit(1);
}
int main(int argc, char *argv[])
{
    if(pipe(ary) == -1) Oops("pipe");
    int pid = fork();
    if(pid == -1){
        Oops("fork");
    }else if(!pid){
        int len = strlen(CHILD_MSG); 
        while(1){
            if(write(ary[1], CHILD_MSG, len) != len) Oops("write");
            sleep(3);
        }
    }else{
        int len = strlen(PAR_MSG);
        while(1){
            if(write(ary[1], PAR_MSG, len) != len) Oops("write");
            sleep(1);
            int bytes = read(ary[0], buf, BUFSIZ);
            if(bytes == -1) Oops("read");
            if(!bytes) break;
            write(1, buf, bytes);
        }
    }
    return 0;
}
