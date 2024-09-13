#include <math.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

#include "cell.h"
#include "panel.h"

int CONTINUE, SPLASH;
const float ROOT3 = 1.732050807f;
const float ROOT3_INV = 0.57735026919f;
const float CURSOR_ASPECT_RATIO = 0.66f;
const float _DH_DW = ROOT3_INV * CURSOR_ASPECT_RATIO;

int _lastchar;
int _rows, _cols;
int _rmid, _cmid;
float _radius;
int _hex_w, _hex_h;

struct Hex *_h;
struct HexCoordinate *_c;


void update_vars(void)
{
    _hex_w = round(_radius * ROOT3 / 2);
    _hex_h = round(_radius * CURSOR_ASPECT_RATIO / 2);

    _c = &(_h->p);
}


int initialise(void)
{
    initscr();              /* init the lib */
    raw();                  /* disable line buffering */
    keypad(stdscr, TRUE);   /* capture special keys */
    noecho();               /* don't echo user input */
    curs_set(0);            /* disable cursor */

    CONTINUE=1;
    SPLASH=1;

    getmaxyx(stdscr, _rows, _cols);
    _rmid = _rows / 2;
    _cmid = _cols / 2;
    _lastchar=0;
    _radius=10;

    _h = create_hex();

    update_vars();

    return 0;
}


void cleanup(void)
{
    endwin();
    destroy_hex(_h);
}


int draw_border(int r0, int c0, int w, int h)
{
    mvhline(r0,     c0,     ACS_HLINE, w-1);
    mvvline(r0,     c0,     ACS_VLINE, h-1);
    mvvline(r0,     c0+w-1, ACS_VLINE, h-1);
    mvhline(r0+h-1, c0,     ACS_HLINE, w-1);

    mvaddch(r0,     c0,     ACS_PLUS);
    mvaddch(r0,     c0+w-1, ACS_PLUS);
    mvaddch(r0+h-1, c0,     ACS_PLUS);
    mvaddch(r0+h-1, c0+w-1, ACS_PLUS);

    return 0;
}


int draw_rectangle(int r0, int c0, int w, int h, char bg)
{
    for (int r=0; r<h; r++) {
        mvhline(r0+r, c0, bg, w);
    }

    return 0;
}


int draw_box(int r0, int c0, int w, int h, char bg)
{
    draw_rectangle(r0, c0, w, h, bg);
    draw_border(r0, c0, w, h);

    return 0;
}


int draw_panel(struct Panel *p, int r0, int c0)
{
    draw_box(r0, c0, p->w, p->h, ' ');

    for (int i=0; i<p->len; i++) {
        if (!p->lines[i]) { continue; }
        mvprintw(r0+2+i, c0+2, "%s", p->lines[i]);
    }
    return 0;
}


int draw_splash(void)
{
    int midrow = (_rows / 2) - 3;
    int midcol = (_cols / 2) - 22;

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


char get_terrainchr(enum TERRAIN t)
{
    switch (t) {
        case WATER:
            return '~';
        case MOUNTAIN:
            return '^';
        case PLAINS:
            return ';';
        case HILL:
            return 'n';
        case FOREST:
            return 'T';
        case DESERT:
            return '*';
        case JUNGLE:
            return '#';
        case SWAMP:
            return 'j';
        default:
            break;
    }
    return '?';
}


int draw_hex(int row0, int col0, struct Hex *hex)
{
    char ch = get_terrainchr(hex->t);
    int dh = 0;

    for (int col = -_hex_w; col <= _hex_w; col++) {
        dh = (col < 0) ? round((_hex_w+col)*_DH_DW) : round((_hex_w-col)*_DH_DW);
        for (int row = -(_hex_h + dh); row <= (_hex_h + dh); row++) {
            mvaddch(row0 + row, col0 + col, ch);
        }
    }
    return 0;
}


int draw_screen(void)
{
    draw_border(0, 0, _cols, _rows);
    draw_hex(_rmid, _cmid, _h);

    if (SPLASH) {
        draw_splash();
    }

    refresh();
    return 0;
}


int handle_input(void)
{
    _lastchar = getch();

    SPLASH = 0;

    switch (_lastchar) {
        case 'Q':
            CONTINUE = 0;
            break;
        default:
            break;
    }

    update_vars();
    return 0;
}


int main(void)
{
    initialise();

    while (CONTINUE) {
        draw_screen();
        handle_input();
    }

    cleanup();
    return 0;
}
