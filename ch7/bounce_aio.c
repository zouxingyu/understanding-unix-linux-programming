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
#include <aio.h>
#define MESSAGE "hello world"
#define BLANK "           "

int row, col, dir, delay, ndelay;
int flag;
struct aiocb aioBuf;
char buf[1];
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
}
void InputHandler(int signum) {
    char *c = aioBuf.aio_buf;
    if(aio_error(&aioBuf)){
        Oops("aio_read");
    }else{
        if(aio_return(&aioBuf) == 1){
            char cc = *c;
            if(cc == 'q')flag = 0;
            else if(cc == ' ')dir = -dir;
            else if(cc == 'f')ndelay = delay / 2;
            else if(cc == 's')ndelay = delay * 2;
            if(ndelay)SetInterval(delay = ndelay);
        }
        aio_read(&aioBuf);
    }
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
    row = 10;
    col = 0;
    dir = 1;
    delay = 500;
    ndelay = 0;
    flag = 1;
    aioBuf.aio_fildes = 0;
    aioBuf.aio_nbytes = 1;
    aioBuf.aio_offset = 0;
    aioBuf.aio_buf = buf;
    aioBuf.aio_sigevent.sigev_notify = SIGEV_SIGNAL;
    aioBuf.aio_sigevent.sigev_signo = SIGIO;
    aio_read(&aioBuf);
    SetInterval(delay);
    while (flag)
        ;
    endwin();
    return 0;
}
