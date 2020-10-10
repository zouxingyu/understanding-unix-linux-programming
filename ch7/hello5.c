#include <curses.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int left = 10;
    int right = 30;
    int startRow = 10;
    int startCol = 10;
    int dir = 1;
    char *str = "hello";
    char *blank = "     ";
    initscr();
    clear();
    while (1) {
        move(startRow, startCol);
        addstr(str);
        move(LINES - 1, COLS - 1);
        refresh();
        sleep(5);
        move(startRow, startCol);
        addstr(blank);
        startCol += dir;
        if (startCol >= right) dir = -1;
        if (startCol <= left) dir = 1;
    }
    return 0;
}
