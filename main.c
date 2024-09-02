#include <ncurses.h>
#include <stdlib.h>

int CONTINUE = 1;
int lastchar = 0;


int init_screen()
{
    initscr();              /* init the lib */
    raw();                  /* disable line buffering */
    keypad(stdscr, TRUE);   /* capture special keys */
    noecho();               /* don't echo user input */
    curs_set(0);            /* disable cursor */

    return 0;
}


int draw_screen()
{
    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    mvhline(0, 0, ACS_HLINE, cols);
    mvvline(1, 0, ACS_VLINE, rows);
    mvvline(0, cols-1, ACS_VLINE, cols);
    mvhline(rows-1, 0, ACS_HLINE, cols);

    mvprintw(1, 1, "Welcome, test");
    mvprintw(2, 2, "Last char was : ");
    attron(A_BOLD);
    printw("%c", lastchar);
    attroff(A_BOLD);

    refresh();
    return 0;
}


int handle_input()
{
    int ch = getch();

    switch (ch) {
        case 'q':
            CONTINUE = 0;
            break;
        default:
            lastchar = ch;
    }

    return 0;
}
    
    
int main()
{
    init_screen();

    while (CONTINUE) {
        draw_screen();
        handle_input();
    }

    endwin();
    return 0;
}
