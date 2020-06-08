#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <utmp.h>

#define SHOWHOST
void show_info(struct utmp*);
void show_time(time_t);
int utmp_open(char* filename);
struct utmp* utmp_next();
void utmp_close();

int main() {
    struct utmp* utmpbuf;
    if (utmp_open(UTMP_FILE) == -1) {
        perror(UTMP_FILE);
        exit(1);
    }
    while ((utmpbuf = utmp_next()) != NULL) {
        show_info(utmpbuf);
    }
    utmp_close();
    return 0;
}
void show_info(struct utmp* ptr) {
    if (ptr->ut_type != USER_PROCESS)
        return;
    printf("%-8.8s", ptr->ut_user);
    printf("%-8.8s", ptr->ut_line);
    show_time(ptr->ut_tv.tv_sec);
#ifdef SHOWHOST
    if (ptr->ut_host[0] != '\0')

        printf("(%s)", ptr->ut_host);
#endif
    printf("\n");
}
void show_time(time_t time) {
    char* cp = ctime(&time);
    printf("%12.12s", cp + 4);
}
