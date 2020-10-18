#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#define BUFSIZE 100
int main(int argc, char *argv[])
{
    char buf[BUFSIZE];
    FILE *p = popen("ps|sort", "r");
    while(fgets(buf, BUFSIZE, p) != NULL){
        printf("%s", buf);
    }
    pclose(p);
    return 0;
}
