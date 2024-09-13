#include <math.h>
#include <ncurses.h>
#include <stdlib.h>

#include "cell.h"

int CONTINUE, SPLASH;
const float ROOT3 = 1.732050807f;
const float ROOT3_INV = 0.57735026919f;
const float CURSOR_ASPECT_RATIO = 0.66f;
const float _DH_DW = ROOT3_INV * CURSOR_ASPECT_RATIO;

int _lastchar;
float _radius;
int _hex_w, _hex_h;

struct Hex *_h;
struct HexCoordinate *_c;


int init_vars(void)
{
    CONTINUE=1;
    SPLASH=1;

    _lastchar=0;
    _radius=10;

    _hex_w = round(_radius * ROOT3 / 2);
    _hex_h = round(_radius * CURSOR_ASPECT_RATIO / 2);

    _h = create_hex();
    _c = &(_h->p);

    return 0;
}


void update_vars(void)
{
    _hex_w = round(_radius * ROOT3 / 2);
    _hex_h = round(_radius * CURSOR_ASPECT_RATIO / 2);

    _c = &(_h->p);
}


void cleanup(void)
{
    free(_h);
}


int init_screen(void)
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


int draw_splash(void)
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
    mvprintw(midrow, midcol, "%s", title);
    attroff(A_BOLD);
    mvprintw(midrow+1, midcol, "%s", line1);
    mvprintw(midrow+2, midcol, "%s", line2);
    mvprintw(midrow+3, midcol, "%s", line3);

    return 0;
}


int draw_screen(void)
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


int handle_input(void)
{
    int ch = getch();

    SPLASH = 0;

    switch (ch) {
        case 'Q':
            CONTINUE = 0;
            break;
        default:
            _lastchar = ch;
    }

    return 0;
}


int main(void)
{
    init_vars();
    init_screen();

    while (CONTINUE) {
        draw_screen();
        handle_input();
    }

    endwin();
    cleanup();
    return 0;
}
