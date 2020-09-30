#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <utmp.h>
#define NRECS 16
#define UTSIZE (sizeof(struct utmp))

static struct utmp utmpbuf[NRECS];
static int num_recs;
static int cur_rec;
static int accu_recs;
static int fd_utmp = -1;

int utmp_open(char *filename) {
    fd_utmp = open(filename, O_RDONLY);
    cur_rec = num_recs = accu_recs = 0;
    return fd_utmp;
}
void utmp_close() {
    if (fd_utmp != -1)
        close(fd_utmp);
}
int utmp_reload() {
    int amt_read;
    amt_read = read(fd_utmp, utmpbuf, NRECS * UTSIZE);
    num_recs = amt_read / UTSIZE;
    cur_rec = 0;
    return num_recs;
}
struct utmp *utmp_next() {
    struct utmp *recp;
    if (fd_utmp == -1)
        return NULL;
    if (cur_rec == num_recs && utmp_reload() == 0)
        return NULL;
    recp = &utmpbuf[cur_rec];
    cur_rec++;
    accu_recs++;
    return recp;
}
off_t utmp_seek(off_t record_offset, int base) {
    if (fd_utmp == -1)
        return -1;
    if (base == SEEK_SET || base == SEEK_END)
        return lseek(fd_utmp, record_offset * UTSIZE, base);
    if (base == SEEK_CUR)
        return lseek(fd_utmp, record_offset * UTSIZE, accu_recs * UTSIZE);
}
