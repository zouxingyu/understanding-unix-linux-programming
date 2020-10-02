#include <dirent.h>
#include <errno.h>
#include <grp.h>
#include <pwd.h>
#include <stdc-predef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define MAXSIZE 128
typedef struct DirQueue{
    int idx; 
    int size;
    char *queue[MAXSIZE];
}DirQueue;
void unix_error(char *strerror);
void do_ls(char *pathname);
void do_stat(char *fullname);
void show_file_info(char *filename, struct stat *ptr);
void mode_to_letters(mode_t mode, char *modestr);
char *uid_to_name(uid_t uid);
char *gid_to_name(gid_t gid);
char *concatenation(char *buf, char *str1, char *str2);
char *GetDir(DirQueue *dPtr);
void SetDir(DirQueue *dPtr, char* dir);
void FreeDir(DirQueue *dPtr);

int main(int argc, char **argv) {
    if (argc == 1) {
        do_ls(".");
    } else {
        while (--argc) {
            do_ls(*++argv);
        }
    }
}
void unix_error(char *strerror) {
    perror(strerror);
    exit(1);
}
void do_ls(char *pathname) {
    printf("%s:\n", pathname);
    DIR *dptr;
    if ((dptr = opendir(pathname)) == NULL) {
        unix_error(pathname);
    }

    struct dirent *next;
    errno = 0;
    DirQueue *ptr = malloc(sizeof(DirQueue));
    ptr->size = 0;
    ptr->idx = 0;
    while ((next = readdir(dptr)) != NULL) {
        char buf[MAXSIZE];
        char *fullname = concatenation(buf, pathname, next->d_name);
        printf("%s ",next->d_name);
        if (next->d_type == DT_DIR && strcmp(next->d_name, ".") && strcmp(next->d_name, "..")) {
           //  do_ls(fullname);
            SetDir(ptr, fullname);                           
          //  printf("/%s/ ",fullname);
        }
   //     do_stat(fullname);
    }
    if (errno != 0) {
        unix_error("read dirent error");
    }
    printf("\n\n");
    char *p;
    while(p = GetDir(ptr)){
      //  printf("!%s\n",p);
        do_ls(p);
    }
    FreeDir(ptr);
    closedir(dptr);
}
void do_stat(char *fullname) {
    struct stat tmp;
    if (stat(fullname, &tmp) == -1) {
        unix_error(fullname);
    }
    show_file_info(fullname, &tmp);
}
void show_file_info(char *filename, struct stat *ptr) {
    char modestr[11];
    mode_to_letters(ptr->st_mode, modestr);
    printf("%s", modestr);
    printf("%4lu ", ptr->st_nlink);
    printf("%-8s ", uid_to_name(ptr->st_uid));
    printf("%-8s ", gid_to_name(ptr->st_gid));
    printf("%8ld ", ptr->st_size);
    printf("%.12s ", 4 + ctime(&ptr->st_mtime));
    printf("%s\n", filename);
}
void mode_to_letters(mode_t mode, char *modestr) {
    strcpy(modestr, "----------");
    if (S_ISDIR(mode))
        modestr[0] = 'd';
    else if (S_ISCHR(mode))
        modestr[0] = 'c';
    else if (S_ISBLK(mode))
        modestr[0] = 'b';
    if (mode & S_IRUSR) modestr[1] = 'r';
    if (mode & S_IWUSR) modestr[2] = 'w';
    if (mode & S_IXUSR) modestr[3] = 'x';
    if (mode & S_IRGRP) modestr[4] = 'r';
    if (mode & S_IWGRP) modestr[5] = 'w';
    if (mode & S_IXGRP) modestr[6] = 'x';
    if (mode & S_IROTH) modestr[7] = 'r';
    if (mode & S_IWOTH) modestr[8] = 'w';
    if (mode & S_IXOTH) modestr[9] = 'x';
}
char *uid_to_name(uid_t uid) {
    static char numstr[10];
    struct passwd *ptr;
    if ((ptr = getpwuid(uid)) == NULL) {
        sprintf(numstr, "%d", uid);
        return numstr;
    }
    return ptr->pw_name;
}
char *gid_to_name(gid_t gid) {
    static char numstr[10];
    struct group *ptr;
    if ((ptr = getgrgid(gid)) == NULL) {
        sprintf(numstr, "%d", gid);
        return numstr;
    }
    return ptr->gr_name;
}

char *concatenation(char *buf, char *str1, char *str2) {
    int len1 = strlen(str1);
    strcpy(buf, str1);
    buf[len1] = '/';
    buf[len1+1] = '\0';
    return strcat(buf, str2);
}

char *GetDir(DirQueue *dPtr){
    if(dPtr->idx < dPtr->size)return dPtr->queue[dPtr->idx++];
    else return NULL;
}
void SetDir(DirQueue *dPtr, char *dir){
    int len = strlen(dir);
    char *tmp = malloc(len + 1);
    strcpy(tmp,dir);
    dPtr->queue[dPtr->size++] = tmp;
}
void FreeDir(DirQueue* dPtr){
    for(int i=0; i<dPtr->size; i++)free(dPtr->queue[i]);
    free(dPtr);
}
