#include <ncursesw/ncurses.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    initscr();
    clear();
    printf("LINES %d COLS %d", LINES, COLS);
    for (int i = 0; i < LINES; i++) {
        move(i, i + 1);
        if (i % 2) standout();
        addstr("hello world");
        if (i % 2) standend();
    }
    refresh();
    getch();
    endwin();
    return 0;
}
