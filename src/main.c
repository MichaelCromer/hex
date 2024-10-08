#include <math.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "queue.h"
#include "grid.h"
#include "panel.h"
#include "draw.h"

enum INPUTMODE {
    CAPTURE,
    NAVIGATE,
    TERRAIN_SELECT
};

bool CONTINUE;
bool SPLASH, TERRAIN_SELECTOR;
const float CURSOR_ASPECT_RATIO = 0.66f;
const float _DH_DW = ROOT3_INV * CURSOR_ASPECT_RATIO;

enum INPUTMODE _input_mode;
int _lastchar;

struct Hex *_h = NULL;
struct Geometry *_g = NULL;

struct Panel *_splash;
struct Panel *_terrain_selector;

void update_vars(void)
{
    /* placeholder */
    return;
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
    _lastchar=0;

    int r0, c0;
    getmaxyx(stdscr, r0, c0);
    _g = geometry_create(10, 0.66f, c0, r0); /* scale, aspect, cols, rows */
    int rmid = geometry_rmid(_g), cmid = geometry_cmid(_g);;

    _splash = panel_create(0, 0, 5);
    panel_add_line(_splash, "Welcome to hex",                           0);
    panel_add_line(_splash, "Use u,i,h,l,n,m to navigate tiles",        2);
    panel_add_line(_splash, "Use j to interact with the current tile",  3);
    panel_add_line(_splash, "Shift-q to exit.",                         4);
    r0 = rmid - (panel_height(_splash) / 2);
    c0 = cmid - (panel_width(_splash) / 2);
    panel_set_rc(_splash, r0, c0);

    _terrain_selector = panel_create(0, 0, 5);
    panel_add_line(_terrain_selector, "Select Terrain:", 0);
    panel_add_line(_terrain_selector, "1. Ocean     2. Mountain 3. Plains", 2);
    panel_add_line(_terrain_selector, "4. Hills     5. Forest   6. Desert", 3);
    panel_add_line(_terrain_selector, "7. Jungle    8. Swamp    q. Close ", 4);
    r0 = rmid - (panel_height(_terrain_selector) / 2);
    c0 = cmid - (panel_width(_terrain_selector) / 2);
    panel_set_rc(_terrain_selector, r0, c0);

    _h = hex_create();

    update_vars();

    return 0;
}


void cleanup(void)
{
    erase();
    endwin();
    panel_destroy(_splash);
    panel_destroy(_terrain_selector);
    hex_destroy(_h);
}


int input_capture(void)
{
    if (SPLASH) {
        SPLASH = false;
    }

    return NAVIGATE;
}


int input_navigate(void)
{
    bool directional = false;

    /* first handle the non-directional keys */
    switch (_lastchar) {
        case 'Q':
            CONTINUE = false;
            break;
        case 'T':
            TERRAIN_SELECTOR = true;
            return TERRAIN_SELECT;
        case 'j': /* TODO change this to open/close info panels */
            break;
        default:
            directional = true;
            break;
    }

    /* exit unless we fell through the above */
    if (!directional) {
        return NAVIGATE;
    }

    static const char *navichar_lower = "kiuhnm";
    static const char *navichar_upper = "KIUHNM";
    int step_count = 0;
    for (int i=0; i<6; i++) {
        if (_lastchar == navichar_upper[i]) {
            step_count = 3;
        } else if (_lastchar == navichar_lower[i]) {
            step_count = 1;
        } else {
            continue;
        }
        while (hex_neighbour(_h, i) && step_count) {
            _h = hex_neighbour(_h, i);
            step_count--;
        }
        break;
    }

    return NAVIGATE;
}


int input_terrain(void)
{
    enum TERRAIN t = NONE;
    switch (_lastchar) {
        case 'q':
            break;
        case '1':
            t = WATER;
            break;
        case '2':
            t = MOUNTAINS;
            break;
        case '3':
            t = PLAINS;
            break;
        case '4':
            t = HILLS;
            break;
        case '5':
            t = FOREST;
            break;
        case '6':
            t = DESERT;
            break;
        case '7':
            t = JUNGLE;
            break;
        case '8':
            t = SWAMP;
            break;
        default:
            return TERRAIN_SELECT;
    }
    if (hex_get_terrain(_h) == NONE) {
        hex_create_neighbours(_h);
    }
    hex_set_terrain(_h, t);
    TERRAIN_SELECTOR = false;
    return NAVIGATE;
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
        draw_screen(_g, _h);
        if (SPLASH) {
            draw_panel(_splash);
        }
        if (TERRAIN_SELECTOR) {
            draw_panel(_terrain_selector);
        }
        handle_input();
    }

    cleanup();
    return 0;
}
