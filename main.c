#include <math.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "grid.h"
#include "panel.h"

enum INPUTMODE {
    CAPTURE,
    NAVIGATE,
    TERRAIN_SELECT
};

bool CONTINUE;
bool SPLASH, TERRAIN_SELECTOR;
const float ROOT3 = 1.732050807f;
const float ROOT3_INV = 0.57735026919f;
const float CURSOR_ASPECT_RATIO = 0.66f;
const float _DH_DW = ROOT3_INV * CURSOR_ASPECT_RATIO;

enum INPUTMODE _input_mode;
int _lastchar;
int _rows, _cols;
int _rmid, _cmid;
float _radius;
int _hex_w, _hex_h;

struct Hex *_h;

struct Panel *_splash;
struct Panel *_terrain_selector;

void update_vars(void)
{
    _hex_w = round(_radius * ROOT3 / 2);
    _hex_h = round(_radius * CURSOR_ASPECT_RATIO / 2);
}


int initialise(void)
{
    initscr();              /* init the lib */
    raw();                  /* disable line buffering */
    keypad(stdscr, TRUE);   /* capture special keys */
    noecho();               /* don't echo user input */
    curs_set(0);            /* disable cursor */

    CONTINUE = true;
    SPLASH = true;
    TERRAIN_SELECTOR = false;

    _input_mode = CAPTURE;

    getmaxyx(stdscr, _rows, _cols);
    _rmid = _rows / 2;
    _cmid = _cols / 2;
    _lastchar=0;
    _radius=10;

    int r0, c0;
    _splash = panel_create(0, 0, 5);
    panel_add_line(_splash, "Welcome to hex",                           0);
    panel_add_line(_splash, "Use u,i,h,l,n,m to navigate tiles",        2);
    panel_add_line(_splash, "Use j to interact with the current tile",  3);
    panel_add_line(_splash, "Shift-q to exit.",                         4);
    r0 = _rmid - (_splash->h / 2);
    c0 = _cmid - (_splash->w / 2);
    panel_set_rc(_splash, r0, c0);

    _terrain_selector = panel_create(0, 0, 5);
    panel_add_line(_terrain_selector, "Select Terrain:", 0);
    panel_add_line(_terrain_selector, "1. Ocean     2. Mountain 3. Plains", 2);
    panel_add_line(_terrain_selector, "4. Hills     5. Forest   6. Desert", 3);
    panel_add_line(_terrain_selector, "7. Jungle    8. Swamp    q. Close ", 4);
    r0 = _rmid - (_terrain_selector->h / 2);
    c0 = _cmid - (_terrain_selector->w / 2);
    panel_set_rc(_terrain_selector, r0, c0);

    _h = hex_create();

    update_vars();

    return 0;
}


void cleanup(void)
{
    endwin();
    panel_destroy(_splash);
    panel_destroy(_terrain_selector);
    hex_destroy(_h);
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


int draw_panel(struct Panel *p)
{
    draw_box(p->r0, p->c0, p->w, p->h, ' ');

    for (int i=0; i<p->len; i++) {
        if (!p->lines[i]) { continue; }
        mvprintw(p->r0+2+i, p->c0+2, "%s", p->lines[i]);
    }
    return 0;
}


int clear_panel(struct Panel *p)
{
    draw_rectangle(p->r0, p->c0, p->w, p->h, ' ');

    return 0;
}


char get_terrainchr(enum TERRAIN t)
{
    switch (t) {
        case WATER:
            return '~';
        case MOUNTAINS:
            return '^';
        case PLAINS:
            return ';';
        case HILLS:
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
        draw_panel(_splash);
    }
    if (TERRAIN_SELECTOR) {
        draw_panel(_terrain_selector);
    }

    refresh();
    return 0;
}


int input_capture(void)
{
    if (SPLASH) {
        clear_panel(_splash);
        SPLASH = false;
    }

    return NAVIGATE;
}


int input_navigate(void)
{
    switch (_lastchar) {
        case 'Q':
            CONTINUE = false;
            break;
        case 'j':
            if ((_h->t) == NONE) {
                TERRAIN_SELECTOR = true;
                return TERRAIN_SELECT;
            }
            break;
        case 'T':
            TERRAIN_SELECTOR = true;
            return TERRAIN_SELECT;
        default:
            break;
    }

    return NAVIGATE;
}


int input_terrain(void)
{
    switch (_lastchar) {
        case 'q':
            clear_panel(_terrain_selector);
            TERRAIN_SELECTOR = false;
            return NAVIGATE;
        default:
            break;
    }
    return TERRAIN_SELECT;
}


void handle_input(void)
{
    _lastchar = getch();

    switch (_input_mode) {
        case CAPTURE:
            _input_mode = input_capture();
            break;
        case NAVIGATE:
            _input_mode = input_navigate();
            break;
        case TERRAIN_SELECT:
            _input_mode = input_terrain();
        default:
            break;
    }

    update_vars();
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
