#include <bits/types/struct_timeval.h>
#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <curses.h>
#include <string.h>
#define MESSAGE "hello world"
#define BLANK   "           "

int row, col, dir;

void Oops(char *str){
    perror(str);
    exit(1);
}
void SetInterval(int milliseconds){
    struct itimerval oldTimer, newTimer;
    newTimer.it_interval.tv_sec = milliseconds / 1000;
    newTimer.it_interval.tv_usec = (milliseconds % 1000) * 1000;
    newTimer.it_value.tv_sec = newTimer.it_interval.tv_sec;
    newTimer.it_value.tv_usec = newTimer.it_interval.tv_usec;
    if(setitimer(ITIMER_REAL, &newTimer, NULL) == -1)Oops("setitimer 1");
    //if(getitimer(ITIMER_REAL, &oldTimer) == -1)Oops("getitimer");
    //int remainTime = oldTimer.it_value.tv_sec * 1000 + oldTimer.it_value.tv_usec / 1000;
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
void AlrmHandler(int signum ){
//    signal(SIGALRM, SetInterval);
    move(row, col);
    addstr(BLANK);
    col += dir;
    move(row, col);
    addstr(MESSAGE);
    refresh();
    if(col <= 0 && dir == -1)dir = 1;
    else if(col + strlen(MESSAGE) >= COLS && dir ==1)dir = -1;
}
int main(int argc, char *argv[])
{
    if(signal(SIGALRM, AlrmHandler) == SIG_ERR)Oops("signal");
    initscr();
    crmode();
    noecho();
    clear();
    row = 10;
    col = 0;
    dir = 1;
    int input;
    int delay = 500, ndelay = 0;
    SetInterval(delay);
    while(1){
        input = getch();
        if(input == 'q')break;
        else if(input == ' ')dir *= -1;
        else if(input == 'f')ndelay = delay / 2;
        else if(input == 's')ndelay = delay * 2;
        if(ndelay > 0)SetInterval(delay = ndelay);
    }
    endwin();
    return  0;
}
