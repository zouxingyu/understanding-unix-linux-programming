#include <ctype.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

#define QUESTIONS "Do you want another transaction?"
#define SLEEPTIME 2
#define MAXTRIES 3
void SaveRestoreMode(int a) {
    static struct termios originalTermios;
    static int originalFlags;
    if (!a) {
        tcgetattr(0, &originalTermios);
        originalFlags = fcntl(0, F_GETFL);
    } else {
        tcsetattr(0, TCSANOW, &originalTermios);
        fcntl(0, F_SETFL, originalFlags);
    }
}
void PrintTermios(struct termios *ptr) {
    if (ptr->c_cflag & ICANON)
        printf("ICANON");
    else
        printf("-ICANON");
    if (ptr->c_cflag & ECHO)
        printf("ECHO");
    else
        printf("-ECHO");
    printf("   ");
}
void SetMode() {
    struct termios ttyState;
    tcgetattr(0, &ttyState);
    // PrintTermios(&ttyState);
    ttyState.c_lflag &= ~ICANON;
    ttyState.c_lflag &= ~ECHO;
    ttyState.c_cc[VMIN] = 1;
    tcsetattr(0, TCSANOW, &ttyState);
    // PrintTermios(&ttyState);
    int fileFlags = fcntl(0, F_GETFL);
    fileFlags |= O_NDELAY;
    fcntl(0, F_SETFL, fileFlags);
}
void SetMode2() {
    struct termios ttyState;
    tcgetattr(0, &ttyState);
    // PrintTermios(&ttyState);
    ttyState.c_lflag &= ~ICANON;
    //    ttyState.c_lflag &= ~ECHO;
    ttyState.c_cc[VMIN] = 0;
    ttyState.c_cc[VTIME] = 20;
    tcsetattr(0, TCSANOW, &ttyState);
}
void SigHandler(int signum) { SaveRestoreMode(1); }
int GetChar() {
    int a;
    while ((a = getchar()) != EOF && strchr("YyNn", a) == NULL)
        ;
    return a;
}
int GetResponse(char *question, int sleepTime, int maxTries) {
    int input;
    printf("%s", question);
    fflush(stdout);
    while (1) {
        //sleep(sleepTime);
        input = tolower(GetChar());
        if (maxTries-- == 0) return 2;
        if (input == 'y') return 0;
        if (input == 'n') return 1;
    }
}
int main(int argc, char *argv[]) {
    int response;
    SaveRestoreMode(0);
    // SetMode();
    SetMode2();
    // signal(SIGINT, SigHandler);
    // signal(SIGQUIT, SIG_IGN);
    response = GetResponse(QUESTIONS, SLEEPTIME, MAXTRIES);
    printf(" %d ", response);
    // int c;
    // int a = 3;
    // char buf[1];
    // while (1) {
    //    printf("-----------------\n");
    //    sleep(1);
    //    if (a-- == 0) break;
    //    while ((c = getchar()) != EOF) {
    //        printf("!!!!!");
    //        if(c == 'y')return 0;
    //    }
    //    printf("-----------------\n");
    //}
    SaveRestoreMode(1);
    return 0;
}
