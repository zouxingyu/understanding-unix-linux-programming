#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#define REPLY "Thanks for sending message"
int MakeInternetAddress(struct sockaddr_in *sattrPtr, char *hostName, int port);
int GetInternetAddress(struct sockaddr_in *sattrPtr, char *address, int *port);
int MakeDgramServerSocket(int port);
int MakeDgramClientSocket();
void ShowSenderAddress(struct sockaddr_in *sattrPtr);
char buf[BUFSIZ];
int main(int argc, char *argv[])
{
    if(argc != 2){
        fprintf(stderr, "usage: dgrecv port\n");
        exit(1);
    }
    int sockId = MakeDgramServerSocket(atoi(argv[1]));
    if(sockId == -1) exit(1);
    struct sockaddr_in saddr;
    int bytes;
    int rlen = strlen(REPLY);
    socklen_t slen = sizeof(saddr);
    while((bytes = recvfrom(sockId, buf, BUFSIZ, 0, (struct sockaddr*)&saddr, &slen)) > 0){
        buf[bytes] = '\0';
        printf("dgrecv: got a message: %s\n", buf);
        ShowSenderAddress(&saddr);
        if(sendto(sockId, REPLY, rlen, 0, (struct sockaddr*)&saddr, slen) == -1)
            perror("sendto");
    }
    if(bytes == -1)
        perror("recvfrom");
        exit(1);
    return 0;
}
