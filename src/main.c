#include <math.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "include/grid.h"
#include "include/panel.h"
#include "include/draw.h"
#include "include/interface.h"


enum INPUTMODE {
    INPUT_NONE,
    INPUT_CAPTURE,
    INPUT_NAVIGATE,
    INPUT_TERRAIN
};


struct StateManager {
    bool quit;
    enum INPUTMODE input_mode;
};


struct StateManager *state_create(void)
{
    struct StateManager *s = malloc(sizeof(struct StateManager));

    s->quit = false;
    s->input_mode = INPUT_NONE;

    return s;
}


void state_destroy(struct StateManager *s)
{
    free(s);
    s = NULL;
    return;
}


struct Map *map = NULL;
struct Geometry *geometry = NULL;
struct UserInterface *ui = NULL;
struct StateManager *sm = NULL;


void update_vars(void)
{
    struct Hex *current_hex = map_curr(map);

    /* TODO this is ridiculous as written here */
    struct Panel *hex_detail = ui_panel(ui, PANEL_DETAIL);
    char *coordinate = malloc(32); /* TODO def an appropriate length */
    snprintf(coordinate, 32,
            "    (%d, %d, %d)",
            hex_p(current_hex),
            hex_q(current_hex),
            hex_r(current_hex)
           );
    panel_remove_line(hex_detail, 1);
    panel_add_line(hex_detail, 1, coordinate);

    char *terrain = malloc(32);
    snprintf(terrain, 32,
            "    Terrain: %s",
            terrain_string(hex_terrain(current_hex))
            );
    panel_remove_line(hex_detail, 2);
    panel_add_line(hex_detail, 2, terrain);

    free(coordinate);
    free(terrain);
    coordinate = NULL;
    terrain = NULL;
    return;
}


int initialise(void)
{
    initscr();              /* init the lib */
    raw();                  /* disable line buffering */
    keypad(stdscr, TRUE);   /* capture special keys */
    noecho();               /* don't echo user input */
    curs_set(0);            /* disable cursor */

    sm = state_create();
    sm->input_mode = INPUT_CAPTURE;

    /* TODO remove magic numbers and have a geometry_initialise that takes a screen */
    int r0, c0;
    getmaxyx(stdscr, r0, c0);
    geometry = geometry_create(10, 0.66f, c0, r0); /* scale, aspect, cols, rows */
    int rmid = geometry_rmid(geometry), cmid = geometry_cmid(geometry);;

    /* TODO remove the jank, make it reference a geometry */
    ui = ui_initialise();
    panel_centre(ui_panel(ui, PANEL_SPLASH), rmid, cmid);
    ui_toggle(ui, PANEL_SPLASH);

    map = map_create(hex_origin());
    map_goto(map, coordinate_zero());

    update_vars();

    return 0;
}


void cleanup(void)
{
    ui_destroy(ui);
    state_destroy(sm);
    geometry_destroy(geometry);
    map_destroy(map);

    erase();
    endwin();
}


enum INPUTMODE input_parse_capture(char ch)
{
    if (!ch) {
        return INPUT_NONE;
    }

    if (ui_show(ui, PANEL_SPLASH)) {
        ui_toggle(ui, PANEL_SPLASH);
    }

    return INPUT_NAVIGATE;
}


enum INPUTMODE input_parse_navigate(char ch)
{
    bool directional = false;

    /* first handle the non-directional keys */
    switch (ch) {
        case 'Q':
            sm->quit = true;
            break;
        case 'T':
            ui_toggle(ui, PANEL_TERRAIN);
            return INPUT_TERRAIN;
        case 'j':
            ui_toggle(ui, PANEL_DETAIL);
            break;
        default:
            directional = true;
            break;
    }

    /* exit unless we fell through the above */
    if (!directional) {
        return INPUT_NAVIGATE;
    }

    static const char *navichar_lower = "kiuhnm";
    static const char *navichar_upper = "KIUHNM";
    int step_count = 0;
    for (int i=0; i<6; i++) {
        if (ch == navichar_upper[i]) {
            step_count = 3;
        } else if (ch == navichar_lower[i]) {
            step_count = 1;
        } else {
            continue;
        }
        while (step_count) {
            map_step(map, i);
            step_count--;
        }
        break;
    }

    return INPUT_NAVIGATE;
}


enum INPUTMODE input_parse_terrain(char ch)
{
    /* first handle direct selection */
    enum TERRAIN t = ch - '0';
    if ((t > NONE) && (t <= SWAMP)) {
        map_paint(map, t);
        return INPUT_TERRAIN;
    }

    /* now handle copy/move painting */
    static const char *brushchar = "KIUHNM";
    for (int i=0; i<6; i++) {
        if (ch != brushchar[i]) {
            /* skip unless input is a motion */
            continue;
        }

        enum TERRAIN t = map_curr_terrain(map);

        if (t == NONE) {
            /* don't paint with none-terrain */
            return INPUT_TERRAIN;
        }

        map_step(map, i);
        map_paint(map, t);

        return INPUT_TERRAIN;
    }

    static const char *navichar_lower = "kiuhnm";
    for (int i=0; i<6; i++) {
        if (ch != navichar_lower[i]) {
            continue;
        }
        map_step(map, i);
    }

    switch (ch) {
        case 'T':
            ui_toggle(ui, PANEL_TERRAIN);
            return INPUT_NAVIGATE;
        default:
            break;
    }
    return INPUT_TERRAIN;
}


void input_parse(char ch)
{
    enum INPUTMODE next_mode = INPUT_NONE;

    switch (sm->input_mode) {
        case INPUT_CAPTURE:
            next_mode = input_parse_capture(ch);
            break;
        case INPUT_NAVIGATE:
            next_mode = input_parse_navigate(ch);
            break;
        case INPUT_TERRAIN:
            next_mode = input_parse_terrain(ch);
            break;
        default:
            break;
    }

    sm->input_mode = next_mode;
    update_vars();
}


int main(void)
{
    initialise();

    while (!sm->quit) {
        draw_screen(geometry, map, ui);
        input_parse(getch());
    }

    cleanup();
    return 0;
}
