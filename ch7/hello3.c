#include <curses.h>
#include <stdio.h>
#include <unistd.h>
int main(int argc, char *argv[]) {
    int i;
    initscr();
    clear();
    for (int i = 0; i < LINES; ++i) {
        move(i, i + 1);
        if (i % 2) standout();
        addstr("hello world");
        if (i % 2) standend();
        sleep(1);
        refresh();
    }
    endwin();
    return 0;
}
