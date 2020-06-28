#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#define MAXSIZE 256
void print_path(ino_t);
ino_t get_inode(char *);
void inum_to_name(ino_t, char *);
int main()
{
    print_path(get_inode("."));
    putchar('\n');
    return 0;
}
void print_path(ino_t inum)
{
    char buf[MAXSIZE];
    if (get_inode("..") != inum)
    {
        chdir("..");
        inum_to_name(inum, buf);
        print_path(get_inode("."));
        printf("/%s", buf);
    }
}
void inum_to_name(ino_t inum, char *buf)
{
    DIR *dptr = opendir(".");
    struct dirent *direntptr;
    if (dptr == NULL)
    {
        perror(".");
        exit(1);
    }
    while ((direntptr = readdir(dptr)) != NULL)
    {
        if (direntptr->d_ino == inum)
        {
            strncpy(buf, direntptr->d_name, MAXSIZE);
            buf[MAXSIZE] = '\0';
            closedir(dptr);
            return;
        }
    }
    fprintf(stderr, "error looking for inum %lu", inum);
    exit(1);
}
ino_t get_inode(char *pathname)
{
    struct stat file;
    if (stat(pathname, &file) == 0)
    {
        return file.st_ino;
    }
    perror(pathname);
    exit(1);
}