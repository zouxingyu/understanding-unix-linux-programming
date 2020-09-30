#include<stdio.h>
#include<time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <utmp.h>
#include<stdlib.h>
#define SHOWHOST
void show_info(struct utmp* ptr); 
void show_time(time_t time); 
int main(int argc, char* argv[]) {
    int fd = open(UTMP_FILE, O_RDONLY);
    if (fd == -1) {
        perror(UTMP_FILE);
        exit(1);
    }
    struct utmp uBuf;
    int count;
    while ((count = read(fd, &uBuf, sizeof(uBuf))) == sizeof(uBuf)) {
        show_info(&uBuf);
    }
    if(count == -1){
        perror("read failed");
    }
    close(fd);
    return 0;
}
void show_info(struct utmp* ptr) {
    if (ptr->ut_type != USER_PROCESS) return;
    printf("%-8.8s", ptr->ut_user);
    printf("%-8.8s", ptr->ut_line);
    show_time(ptr->ut_tv.tv_sec);
#ifdef SHOWHOST
    if (ptr->ut_host[0] != '\0') printf(" (%s)", ptr->ut_host);
#endif
    printf("\n");
}
void show_time(time_t time) {
    struct tm* tPtr= localtime(&time);
   printf("%d-%d-%d %d:%d",tPtr->tm_year+1900,tPtr->tm_mon+1,tPtr->tm_mday,tPtr->tm_hour,tPtr->tm_min); 
}

