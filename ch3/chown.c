#include <ctype.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "%s usage: username/uid file", *argv);
        exit(1);
    }
    bool isUid = false;
    uid_t uid;
    if (isdigit(argv[1][0])) {
        isUid = true;
    }
    if (isUid) {
        uid = atol(argv[1]);
        if(!getpwuid(uid)){
            perror("getpwuid failed");
            exit(1);
        }
    } else {
        struct passwd *ptr = getpwnam(argv[1]);
        if (!ptr) {
            perror("getpwnam failed");
            exit(1);
        } else {
            uid = ptr->pw_uid;
        }
    }
//    printf("%u\n",uid);
    for (int i = 2; i < argc; ++i) {
        char *pathname = argv[i];
        if (chown(pathname, uid, -1) == -1) {
            perror("chowm failed");
            exit(1);
        }
    }
    return 0;
}
