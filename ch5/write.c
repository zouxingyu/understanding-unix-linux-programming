#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <utmp.h>
#include <time.h>
#include <string.h>

#define BUFSIZE 128
void SyscallError(char *str) {
    perror(str);
    exit(1);
}

void Prompt(){
    char *name = getlogin();
    char hostBuf[BUFSIZE];
    gethostname(hostBuf, BUFSIZE);
    char *tty = ttyname(1);
    time_t curTime = time(NULL);
    struct tm *tmPtr = localtime(&curTime);
    printf("\nMessage from %s@%s on %s at %2d:%02d ...\n",name, hostBuf, tty,tmPtr->tm_hour, tmPtr->tm_min);
}
int GetTTY(char *username, char *tty){
    int fd;
    if((fd = open(UTMP_FILE, O_RDONLY)) == -1){
        SyscallError("open utmp file failed");
    }
    struct utmp utmpBuf;
    while(read(fd, &utmpBuf, sizeof(struct utmp)) != -1){
        if(!strcmp(utmpBuf.ut_user, username)){
            strcpy(tty, "/dev/");
            strcat(tty, utmpBuf.ut_line);
            return 0;
        }       
    }
    return -1;
}
int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "usage: write user [tty]\n");
        exit(1);
    }
    char *username = argv[1];
    char tty[UT_LINESIZE + 10];
    Prompt();
    if(GetTTY(username, tty) == -1){
        fprintf(stderr, "write: %s is not logged in", username);
        exit(1);
    } 
    printf("%s\n", tty);
    int fd;
   // strcpy(tty, "/dev/pts/0");
    if((fd = open(tty, O_WRONLY)) == -1){
        SyscallError("open failed");
    }
    char buf[BUFSIZE];
    while(fgets(buf, BUFSIZE, stdin)){
        if(write(fd, buf, strlen(buf)) == -1){
            SyscallError("write failed");
        } 
    }
    close(fd);
    return 0;
}
