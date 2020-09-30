#include<stdio.h>
#include<pwd.h>
#include<unistd.h>
#include<sys/types.h>

int main(int argc, char *argv[])
{
    uid_t uid=geteuid();
    struct passwd * pwdPtr=getpwuid(uid);
    puts(pwdPtr->pw_name);
    return 0;
}
