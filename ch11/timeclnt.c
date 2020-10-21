#include <netdb.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define SYSERROR(str) \
    do {              \
        perror(str);  \
        exit(1);      \
    } while (0)
#define BUFSIZE 256
char buf[BUFSIZE];
int main(int argc, char *argv[]) {
    struct sockaddr_in saddr;
    struct hostent *hp;
    int sockId;

    sockId = socket(AF_INET,SOCK_STREAM, 0 );
    if(sockId == -1)SYSERROR("socket");

    memset(&saddr, 0, sizeof(saddr));
    hp = gethostbyname(argv[1]);
    memcpy(&saddr.sin_addr, hp->h_addr, hp->h_length);
    saddr.sin_port = htons(atoi(argv[2]));
    saddr.sin_family = AF_INET;

    if(connect(sockId, (struct sockaddr*)&saddr, sizeof(saddr)) == -1)
        SYSERROR("connect");

    int bytes;
    bytes = read(sockId, buf, BUFSIZE);
    if(bytes == -1)
        SYSERROR("read");
    if(write(STDOUT_FILENO, buf, bytes) != bytes)
        SYSERROR("write");
    close(sockId);
    return 0;
}
