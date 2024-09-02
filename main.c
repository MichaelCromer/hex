#include <ncurses.h>

int main()
{
    initscr();              /* init the lib */
    raw();                  /* disable line buffering */
    keypad(stdscr, TRUE);   /* capture special keys */
    noecho();               /* don't echo user input */
    curs_set(0);            /* disable cursor */
    
    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    mvhline(0, 0, ACS_HLINE, cols);
    mvvline(1, 0, ACS_VLINE, rows);
    mvvline(0, cols-1, ACS_VLINE, cols);
    mvhline(rows-1, 0, ACS_HLINE, cols);
    mvprintw(1, 1, "Welcome, test");

    refresh();

    getch();

    endwin();
    return 0;
}
