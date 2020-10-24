#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
int MakeInternetAddress(struct sockaddr_in *sattrPtr, char *hostName, int port);
int GetInternetAddress(struct sockaddr_in *sattrPtr, char *address, int *port);
int MakeDgramServerSocket(int port);
int MakeDgramClientSocket();
void ShowSenderAddress(struct sockaddr_in *sattrPtr);
char buf[BUFSIZ];
int main(int argc, char *argv[])
{
    if(argc != 4){
        fprintf(stderr, "usage: dgsend hostname port msg\n");
        exit(1);
    }
    char *msg = argv[3];
    int sockId = MakeDgramClientSocket();
    if(sockId == -1) exit(1);
    struct sockaddr_in saddr;
    MakeInternetAddress(&saddr, argv[1], atoi(argv[2]));
    int bytes;
    if((bytes = sendto(sockId, msg, strlen(msg), 0, (struct sockaddr*) &saddr, sizeof(saddr))) == -1){
        perror("sendto");
    }
    struct sockaddr_in raddr;
    socklen_t slen = sizeof(raddr);
    if((bytes = recvfrom(sockId, buf, BUFSIZ, 0, (struct sockaddr*)&raddr, &slen)) > 0 ){
        printf("dgsend: got a message: %s\n", buf);
        ShowSenderAddress(&raddr);
    }
    if(bytes == -1)
        perror("recvfrom");
    return 0;
}
