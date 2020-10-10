#include <curses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <ncursesw/ncurses.h>
int main(int argc, char *argv[]) {
    initscr();

    clear();
    move(10, 20);
    addstr("Hello, World");
    move(LINES - 1, 0);
    refresh();
    getch();
    endwin();
    return 0;
}
