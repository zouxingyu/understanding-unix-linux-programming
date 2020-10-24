#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#define HOSTLEN 128
#define BACKLOG 1
int MakeServerSocket(int port);
int MakeServerSocketBack(int port, int backLog);
int ConnectToServer(char *hostName, int port);

int MakeServerSocket(int port) { return MakeServerSocketBack(port, BACKLOG); }
int MakeServerSocketBack(int port, int backLog) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1){
        perror("socket");
        return -1;
    }
    struct sockaddr_in sattr;
    char hostName[HOSTLEN];
    gethostname(hostName, HOSTLEN);
    struct hostent *hostentPtr = gethostbyname(hostName);
    if (hostentPtr == NULL){
        fprintf(stderr, "gethostbyname error\n");
        return -1;
    }
    memset(&sattr, 0, sizeof(sattr));
    memcpy(&sattr.sin_addr, hostentPtr->h_addr, hostentPtr->h_length);
    sattr.sin_family = AF_INET;
    sattr.sin_port = htons(port);
    if (bind(fd, (struct sockaddr *)&sattr, sizeof(struct sockaddr)) == -1){
        perror("bind");
        return -1;
    }
    if (listen(fd, backLog) == -1){
        perror("listen");
        return -1;
    }
    return fd;
}
int ConnectToServer(char *hostName, int port) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1){
        perror("socket");
        return -1;
    }
    struct sockaddr_in sattr;
    struct hostent *hostentPtr = gethostbyname(hostName);
    if (hostentPtr == NULL){
        fprintf(stderr, "gethostbyname error\n");
        return -1;
    }
    memset(&sattr, 0, sizeof(sattr));
    memcpy(&sattr.sin_addr, hostentPtr->h_addr, hostentPtr->h_length);
    sattr.sin_family = AF_INET;
    sattr.sin_port = htons(port);
    if (connect(fd, (struct sockaddr *)&sattr, sizeof(struct sockaddr)) == -1){
        perror("connect");
        return -1;
    }
    return fd;
}
