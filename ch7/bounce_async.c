#include <curses.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#define MESSAGE "hello world"
#define BLANK "           "

int row, col, dir, delay, ndelay;
int flag;
void Oops(char *str) {
    perror(str);
    exit(1);
}

void SetInterval(int milliseconds) {
    struct itimerval oldTimer, newTimer;
    newTimer.it_interval.tv_sec = milliseconds / 1000;
    newTimer.it_interval.tv_usec = (milliseconds % 1000) * 1000;
    newTimer.it_value.tv_sec = newTimer.it_interval.tv_sec;
    newTimer.it_value.tv_usec = newTimer.it_interval.tv_usec;
    if (setitimer(ITIMER_REAL, &newTimer, NULL) == -1) Oops("setitimer 1");
    // if(getitimer(ITIMER_REAL, &oldTimer) == -1)Oops("getitimer");
    // int remainTime = oldTimer.it_value.tv_sec * 1000 +
    // oldTimer.it_value.tv_usec / 1000;
    // if(milliseconds > remainTime){
    //     raise(SIGALRM);
    //     if(setitimer(ITIMER_REAL, &newTimer, NULL) == -1)Oops("setitimer 1");
    // }else{
    //     int tmp = remainTime - milliseconds;
    //     newTimer.it_value.tv_sec = tmp / 1000;
    //     newTimer.it_value.tv_usec = (tmp % 1000) * 1000;
    //     if(setitimer(ITIMER_REAL, &newTimer, NULL) == -1)Oops("setitimer 2");
    // }
}
void InputHandler(int signum) {
    // addstr("!!");
    int c = getch();
    if (c == 'q')
        flag = 0;
    else if (c == ' ')
        dir = -dir;
    else if (c == 'f')
        ndelay = delay / 2;
    else if (c == 's')
        ndelay = delay * 2;
    if (ndelay) SetInterval(delay = ndelay);
}
void AlrmHandler(int signum) {
    //    signal(SIGALRM, SetInterval);
    move(row, col);
    addstr(BLANK);
    col += dir;
    move(row, col);
    addstr(MESSAGE);
    refresh();
    if (col <= 0 && dir == -1)
        dir = 1;
    else if (col + strlen(MESSAGE) >= COLS && dir == 1)
        dir = -1;
}
int main(int argc, char *argv[]) {
    initscr();
    clear();
    crmode();
    noecho();
    signal(SIGALRM, AlrmHandler);
    signal(SIGIO, InputHandler);
    if (fcntl(0, F_SETOWN, getpid()) == -1) Oops("fcntl");
    int status = fcntl(0, F_GETFL);
    if (fcntl(0, F_SETFL, status | O_ASYNC) == -1) Oops("fcntl");
    row = 10;
    col = 0;
    dir = 1;
    delay = 500;
    ndelay = 0;
    flag = 1;
    SetInterval(delay);
    while (flag)
        ;
    endwin();
    return 0;
}
