#include <math.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "grid.h"
#include "panel.h"
#include "draw.h"

enum INPUTMODE {
    CAPTURE,
    NAVIGATE,
    TERRAIN_SELECT
};

bool CONTINUE;
bool SPLASH, TERRAIN_SELECTOR, HEX_DETAIL;

enum INPUTMODE input_mode;
int lastchar;

struct Hex *current_hex = NULL;
struct Hex *map = NULL;
struct Geometry *geometry = NULL;

struct Panel *splash = NULL;
struct Panel *terrain_selector = NULL;
struct Panel *hex_detail = NULL;

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

    input_mode = CAPTURE;
    lastchar=0;

    int r0, c0;
    getmaxyx(stdscr, r0, c0);
    geometry = geometry_create(10, 0.66f, c0, r0); /* scale, aspect, cols, rows */
    int rmid = geometry_rmid(geometry), cmid = geometry_cmid(geometry);;

    splash = panel_splash(rmid, cmid);
    terrain_selector = panel_terrain_selector();
    hex_detail = panel_hex_detail();

    current_hex = hex_origin();
    map = current_hex;

    update_vars();

    return 0;
}


void cleanup(void)
{
    erase();
    endwin();
    panel_destroy(splash);
    panel_destroy(terrain_selector);
    panel_destroy(hex_detail);
    hex_destroy(current_hex);
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
    switch (lastchar) {
        case 'Q':
            CONTINUE = false;
            break;
        case 'T':
            TERRAIN_SELECTOR = true;
            return TERRAIN_SELECT;
        case 'j':
            HEX_DETAIL = !HEX_DETAIL;
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
        if (lastchar == navichar_upper[i]) {
            step_count = 3;
        } else if (lastchar == navichar_lower[i]) {
            step_count = 1;
        } else {
            continue;
        }
        while (hex_neighbour(map, current_hex, i) && step_count) {
            current_hex = hex_neighbour(map, current_hex, i);
            step_count--;
        }
        break;
    }

    return NAVIGATE;
}


int input_terrain(void)
{
    /* first handle direct selection */
    enum TERRAIN t = lastchar - '0';
    if ((t > NONE) && (t <= SWAMP)) {
        if (hex_terrain(current_hex) == NONE) {
            hex_create_neighbours(&map, current_hex);
        }
        hex_set_terrain(current_hex, t);
        return TERRAIN_SELECT;
    }

    /* now handle copy/move painting */
    static const char *brushchar = "KIUHNM";
    for (int i=0; i<6; i++) {
        if (lastchar != brushchar[i]) {
            /* skip unless input is a motion */
            continue;
        }

        enum TERRAIN t = hex_terrain(current_hex);
        struct Hex *nbr = hex_neighbour(map, current_hex, i);

        if ((t == NONE) || (nbr == NULL)) {
            /* don't paint with none-terrain */
            return TERRAIN_SELECT;
        }

        current_hex = nbr;
        if (hex_terrain(current_hex) == NONE) {
            hex_create_neighbours(&map, current_hex);
        }
        hex_set_terrain(current_hex, t);

        return TERRAIN_SELECT;
    }

    static const char *navichar_lower = "kiuhnm";
    for (int i=0; i<6; i++) {
        if (lastchar != navichar_lower[i]) {
            continue;
        }
        if (hex_neighbour(map, current_hex, i)) {
            current_hex = hex_neighbour(map, current_hex, i);
            return TERRAIN_SELECT;
        }
    }

    switch (lastchar) {
        case 'T':
            TERRAIN_SELECTOR = false;
            return NAVIGATE;
        default:
            return TERRAIN_SELECT;
    }
    if (hex_terrain(current_hex) == NONE) {
        hex_create_neighbours(&map, current_hex);
    }
    return TERRAIN_SELECT;
}


void handle_input(void)
{
    lastchar = getch();

    switch (input_mode) {
        case CAPTURE:
            input_mode = input_capture();
            break;
        case NAVIGATE:
            input_mode = input_navigate();
            break;
        case TERRAIN_SELECT:
            input_mode = input_terrain();
        default:
            break;
    }

    update_vars();
}


int main(void)
{
    initialise();

    while (CONTINUE) {
        draw_screen(geometry, map, current_hex);
        if (SPLASH) {
            draw_panel(splash);
        }
        if (TERRAIN_SELECTOR) {
            draw_panel(terrain_selector);
        }
        if (HEX_DETAIL) {
            draw_panel(hex_detail);
        }
        handle_input();
    }

    cleanup();
    return 0;
}
