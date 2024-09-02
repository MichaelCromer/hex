#include <ncurses.h>
#include <stdlib.h>

#include "cell.h"

int CONTINUE = 1;
int SPLASH = 1;

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

int draw_border(int x0, int y0, int rows, int cols)
{
    mvaddch(x0,             y0,             ACS_PLUS);
    mvaddch(x0,             y0 + cols-1,    ACS_PLUS);
    mvaddch(x0 + rows-1,    y0,             ACS_PLUS);
    mvaddch(x0 + rows-1,    y0 + cols-1,    ACS_PLUS);

    mvhline(x0,             y0 + 1,         ACS_HLINE, cols-2);
    mvvline(x0 + 1,         y0,             ACS_VLINE, rows-2);
    mvvline(x0 + 1,         y0 + cols-1,    ACS_VLINE, rows-2);
    mvhline(x0 + rows-1,    y0 + 1,         ACS_HLINE, cols-2);

    return 0;
}


int draw_splash()
{
    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    int midrow = (rows / 2) - 3;
    int midcol = (cols / 2) - 22;

    char *title = "Welcome to hex";
    char *line1 = "Use u,i,h,l,n,m to navigate tiles";
    char *line2 = "Use j to interact with the current tile";
    char *line3 = "Shift-q to exit.";

    draw_border(midrow-1, midcol-1, 6, 42);
    attron(A_BOLD);
    mvprintw(midrow, midcol, title);
    attroff(A_BOLD);
    mvprintw(midrow+1, midcol, line1);
    mvprintw(midrow+2, midcol, line2);
    mvprintw(midrow+3, midcol, line3);

    return 0;
}


int draw_screen()
{
    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    draw_border(0, 0, rows, cols);

    if (SPLASH) {
        draw_splash();
    }

    refresh();
    return 0;
}


int handle_input()
{
    int ch = getch();

    SPLASH = 0;

    switch (ch) {
        case 'Q':
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
