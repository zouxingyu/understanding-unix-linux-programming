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
#include <arpa/inet.h>

#define PORTNUM 13000
#define HOSTLEN 256
#define SYSERROR(str) \
    do {              \
        perror(str);  \
        exit(1);      \
    } while (0)

int main(int argc, char *argv[]) {
    struct sockaddr_in saddr, client;
    struct hostent *hp;
    char hostname[HOSTLEN];
    int sockId, sockFd;
    FILE *sockFp;
    time_t cur;
    socklen_t len;

    sockId = socket(AF_INET, SOCK_STREAM, 0);
    if (sockId == -1) SYSERROR("socket");

    memset(&saddr, 0, sizeof(saddr));
    gethostname(hostname, HOSTLEN);
    hp = gethostbyname(hostname);
    memcpy(&saddr.sin_addr, hp->h_addr, hp->h_length);
    saddr.sin_port = htons(PORTNUM);
    saddr.sin_family = AF_INET;
    if (bind(sockId, (struct sockaddr *)&saddr, sizeof(saddr)) == -1)
        SYSERROR("bind");

    if (listen(sockId, 1) == -1) SYSERROR("listen");

    while (1) {
        len = sizeof(client);
        sockFd = accept(sockId, (struct sockaddr*)&client,&len); 
        if (sockFd == -1) SYSERROR("accept");
        sockFp = fdopen(sockFd, "w");
        if (!sockFp) SYSERROR("fdopen");
        cur = time(NULL);
        fprintf(sockFp, "connect from host %s, port %hu\n", inet_ntoa(client.sin_addr), ntohs(PORTNUM));
        fprintf(sockFp, "The time here is ...");
        fprintf(sockFp, "%s", ctime(&cur));
        fclose(sockFp);
    }
    return 0;
}
