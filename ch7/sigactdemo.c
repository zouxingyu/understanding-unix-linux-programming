#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
void Handler(int signum) {
    printf("Entering Handler\n");
    sleep(2);
    printf("Leaving Handler\n");
}
int main(int argc, char *argv[]) {
    char buf[128];
    struct sigaction newAction;
    sigset_t blocked;
    sigemptyset(&blocked);
    sigaddset(&blocked, SIGQUIT);
    newAction.sa_mask = blocked;
    newAction.sa_handler = Handler;
    newAction.sa_flags = SA_NODEFER;
    if (sigaction(SIGINT, &newAction, NULL) == -1) {
        perror("sigaction");
    } else {
        int nchars;
        while (1) {
            printf("Type words:\n");
            if ((nchars = read(0, buf, 128)) == -1) {
                perror("read errror");
            } else {
                buf[nchars] = '\0';
                printf("input: %s", buf);
            }
        }
    }

    return 0;
}
