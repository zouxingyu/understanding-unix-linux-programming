#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#define INPUTLEN 100

void IntHandler(int s) {
    printf("Entering IntHandler...\n");
    sleep(2);
    printf("Leaving IntHandler\n");
}

void QuitHandler(int s) {
    printf("Entering QuitHandler...\n");
    sleep(2);
    printf("Leaving QuitHandler\n");
}

int main(int argc, char *argv[]) {
    signal(SIGINT, IntHandler);
    signal(SIGQUIT, QuitHandler);
    char input[INPUTLEN];
    int nchars;
    do {
        printf("Type a message\n");
        nchars = read(0, input, INPUTLEN - 1);
        if (nchars == 1) {
            perror("read return error");
        } else {
            input[nchars] = '\0';
            printf("You typed: %s", input);
        }

    } while (strcmp(input, "quit"));
    return 0;
}
