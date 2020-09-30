#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <utmp.h>
#include<time.h>
#include<stdlib.h>
void ErrorHandler(char *str) {
    perror(str);
    exit(1);
}
void ShowInfo(struct utmp *ptr) {
     if (ptr->ut_type != USER_PROCESS) return;
    printf("%-8.8s", ptr->ut_user);
    printf("%-8.8s", ptr->ut_line);
    ShowTime(ptr->ut_tv.tv_sec);
#ifdef SHOWHOST
    if (ptr->ut_host[0] != '\0') printf(" (%s)", ptr->ut_host);
#endif
    printf("\n");
}
void ShowTime(time_t time) {
    struct tm *tPtr = localtime(&time);
    printf("%d-%d-%d %d:%d", tPtr->tm_year + 1900, tPtr->tm_mon + 1,
           tPtr->tm_mday, tPtr->tm_hour, tPtr->tm_min);
}

int main(int argc, char *argv[]) {
    struct utmp uBuf;
    int fd = open(WTMP_FILE, O_RDONLY);
    if (fd == -1) ErrorHandler("open failed");
    int offset;
    if ((offset = lseek(fd, 0, SEEK_END)) == -1) ErrorHandler("seek failed");
    while (offset) {
        if (lseek(fd, -sizeof(uBuf), SEEK_CUR) == -1)
            ErrorHandler("seek failed");
        if (read(fd, &uBuf, sizeof(uBuf)) == -1) ErrorHandler("read failed");
        ShowInfo(&uBuf);
        if ((offset = lseek(fd, -sizeof(uBuf), SEEK_CUR)) == -1)
            ErrorHandler("seek failed");
    }

    return 0;
}
