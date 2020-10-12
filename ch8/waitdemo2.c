#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#define SLEEP 2
void ChildProcess(int pid){
    printf("[child process] pid: %d\n", getpid());
    sleep(SLEEP);
//    kill(getpid(), SIGKILL);
    exit(2);
}
void ParentProcess(int pid){
    printf("[parent process]\n");
    int ret, waitStatus;
//    kill(pid, SIGINT);
    ret = wait(&waitStatus);
    printf("!!!!\n");
    int exitNum = waitStatus >> 8;
    int coreNum = waitStatus & 0x80;
    int sigNum = waitStatus & 0x7F;
    printf("[parent process] child process Pid: %d, waitStatus[exit: %d, core: %d, sig: %d]\n", pid, exitNum, coreNum, sigNum);
}
void ChildHandler(int signum){
    printf("Entering ChildHandler\n");
    printf("Leaving ChildHandler\n");
    exit(1);
}
int main(int argc, char *argv[])
{
    printf("[parent process] pid: %d\n", getpid());
    signal(SIGCHLD, ChildHandler);
    int newPid;
    if((newPid = fork()) == -1){
        perror("fork");
        exit(1);
    } 
    if(newPid == 0){
        ChildProcess(newPid);
    }else{
        ParentProcess(newPid);
    }
    return 0;
}
