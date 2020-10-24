#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#define LEN 128
int MakeInternetAddress(struct sockaddr_in *sattrPtr, char *hostName, int port){
    struct hostent *hPtr = gethostbyname(hostName);
    if(hPtr == NULL)
        return -1;
    memset(sattrPtr, 0, sizeof(struct sockaddr_in));
    memcpy(&sattrPtr->sin_addr, hPtr->h_addr, hPtr->h_length);
    sattrPtr->sin_family = AF_INET;
    sattrPtr->sin_port = htons(port);
    return 0;
}
int GetInternetAddress(struct sockaddr_in *sattrPtr, char *address, int len, int *port){
    strncpy(address, inet_ntoa(sattrPtr->sin_addr), len);
    *port = ntohs(sattrPtr->sin_port);
    return 0;
}
void ShowSenderAddress(struct sockaddr_in *sattrPtr){
    char address[LEN];
    int port;
    if(GetInternetAddress(sattrPtr, address, LEN, &port) == -1){
        fprintf(stderr, "GetInternetAddress error\n");
        return;
    }
    printf("from: %s, %d\n", address, port);
} 
int MakeDgramServerSocket(int port){
    int sockId = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockId == -1){
        perror("socket");
        return -1;
    }    
    struct sockaddr_in saddr;
    char hostName[LEN];
    gethostname(hostName, LEN); 
    if(MakeInternetAddress(&saddr, hostName,  port) == -1)
       return -1; 
    if(bind(sockId, (struct sockaddr*)&saddr, sizeof(saddr)) == -1){
        perror("bind");
        return -1;
    }
    return sockId; 
}
int MakeDgramClientSocket(){
    return socket(AF_INET, SOCK_DGRAM, 0);
}
