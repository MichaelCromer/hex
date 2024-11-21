#include <ctype.h>
#include <math.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "include/grid.h"
#include "include/panel.h"
#include "include/draw.h"
#include "include/interface.h"
#include "include/key.h"
#include "include/terrain.h"
#include "include/state.h"


const char *modestr_navigate = "NAV";
const char *modestr_terrain = "TRN";
const char *modestr_command = "CMD";
const char *modestr_unknown = "???";

const char *modestr(enum INPUTMODE m)
{
    switch (m) {
        case INPUT_NAVIGATE:
            return modestr_navigate;
        case INPUT_TERRAIN:
            return modestr_terrain;
        case INPUT_COMMAND:
            return modestr_command;
        default:
            return modestr_unknown;
    }
}

int mode_colour(enum INPUTMODE m)
{
    switch (m) {
        case INPUT_NAVIGATE:
            return COLOR_WHITE;
        case INPUT_TERRAIN:
            return COLOR_GREEN;
        case INPUT_COMMAND:
            return COLOR_RED;
        default:
            return COLOR_WHITE;
    }
}


void draw_statusline(struct StateManager *sm)
{
    int r0 = geometry_rows(state_geometry(sm))-1,
        c0 = 0,
        w  = geometry_cols(state_geometry(sm))-1;

    mvhline(r0, c0, ' ', w);
    attron(COLOR_PAIR(mode_colour(state_mode(sm))));
    mvaddstr(r0, c0+1, modestr(state_mode(sm)));
    attroff(COLOR_PAIR(mode_colour(state_mode(sm))));
    if (state_mode(sm) == INPUT_COMMAND) {
        addch(' ');
        addch(':');
        addstr(state_charbuf(sm));
    }
    return;
}

struct StateManager *state = NULL;

void update_vars(void)
{
    struct Hex *current_hex = map_curr(state_map(state));

    /* TODO this is ridiculous as written here */
    struct Panel *hex_detail = ui_panel(state_ui(state), PANEL_DETAIL);
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
            terrain_name(hex_terrain(current_hex))
            );
    panel_remove_line(hex_detail, 2);
    panel_add_line(hex_detail, 2, terrain);

    int seed = hex_seed(current_hex);
    char *seedstr = malloc(32);
    snprintf(seedstr, 32,
            "  Seed: %d",
            seed
            );
    panel_remove_line(hex_detail, 3);
    panel_add_line(hex_detail, 3, seedstr);

    free(coordinate);
    free(terrain);
    free(seedstr);
    coordinate = NULL;
    terrain = NULL;
    seedstr = NULL;
    return;
}


int initialise(void)
{
    initscr();                  /* init the lib */
    cbreak();                   /* send by char (raw ignores ssigs! use cbreak) */
    noecho();                   /* don't echo user input */
    keypad(stdscr, TRUE);       /* capture special keys */
    intrflush(stdscr, FALSE);   /* interpret screen changes from ssigs correctly*/
    curs_set(0);                /* disable cursor */

    state = state_create();
    state_initialise(state);
    update_vars();

    return 0;
}


void cleanup(void)
{
    state_destroy(state);

    erase();
    endwin();
}



enum INPUTMODE input_parse_capture(void)
{
    if (ui_show(state_ui(state), PANEL_SPLASH)) {
        ui_toggle(state_ui(state), PANEL_SPLASH);
    }

    return INPUT_NAVIGATE;
}


enum INPUTMODE input_parse_navigate(key ch)
{
    /* first handle the non-directional keys */
    switch (ch) {
        case KEY_TOGGLE_COMMAND:
            return INPUT_COMMAND;
        case KEY_TOGGLE_TERRAIN:
            ui_toggle(state_ui(state), PANEL_TERRAIN);
            return INPUT_TERRAIN;
        case KEY_AWAIT_TERRAIN:
            state_set_await(state, true);
            return INPUT_TERRAIN;
        case KEY_TOGGLE_DETAIL:
            ui_toggle(state_ui(state), PANEL_DETAIL);
            return INPUT_NAVIGATE;
        default:
            break;
    }

    if (key_is_direction(ch)) {
        enum DIRECTION d = key_direction(ch);
        int step_count = (key_is_special(ch)) ? 3 : 1;
        while (step_count) {
            map_step(state_map(state), d);
            step_count--;
        }
    }

    return INPUT_NAVIGATE;
}


enum INPUTMODE input_parse_terrain(key ch)
{
    if (state_await(state)) {
        state_set_await(state, false);
        if (key_is_terrain(ch)) {
            map_paint(state_map(state), key_terrain(ch));
        }
        return INPUT_NAVIGATE;
    }
    /* first handle direct selection */

    if (key_is_terrain(ch)) {
        map_paint(state_map(state), key_terrain(ch));
        return INPUT_TERRAIN;
    }

    if (key_is_direction(ch)) {
        enum DIRECTION d = key_direction(ch);
        enum TERRAIN t = map_curr_terrain(state_map(state));
        map_step(state_map(state), d);

        if (key_is_special(ch)) {
            if (t != TERRAIN_UNKNOWN) {
                map_paint(state_map(state), t);
            }
        }
        return INPUT_TERRAIN;
    }

    switch (ch) {
        case KEY_TOGGLE_TERRAIN:
            ui_toggle(state_ui(state), PANEL_TERRAIN);
            return INPUT_NAVIGATE;
        default:
            break;
    }
    return INPUT_TERRAIN;
}


enum INPUTMODE input_parse_command(key ch)
{
    if (ch == KEY_ENTER || ch == '\n') {
        char *buf = state_charbuf(state);
        if (buf[0] == KEY_TOGGLE_QUIT) {
            state_set_quit(state, true);
        }
        state_reset_charbuf(state);
        update_vars();
        return INPUT_NAVIGATE;
    } else if (ch == KEY_BACKSPACE || ch == '\b' || ch == 127) {
        if (strlen(state_charbuf(state)) == 0) {
            state_reset_charbuf(state);
            update_vars();
            return INPUT_NAVIGATE;
        }
        state_reset_nextchar(state);
        update_vars();
    } else if (isprint(ch)) {
        state_set_nextchar(state, ch);
    }

    return INPUT_COMMAND;
}


void input_parse(key ch)
{
    enum INPUTMODE next_mode = INPUT_NONE;

    switch (state_mode(state)) {
        case INPUT_CAPTURE:
            next_mode = input_parse_capture();
            break;
        case INPUT_NAVIGATE:
            next_mode = input_parse_navigate(ch);
            break;
        case INPUT_TERRAIN:
            next_mode = input_parse_terrain(ch);
            break;
        case INPUT_COMMAND:
            next_mode = input_parse_command(ch);
            break;
        default:
            break;
    }

    state_set_mode(state, next_mode);
    update_vars();
}


int main(void)
{
    initialise();

    while (!state_quit(state)) {
        draw_screen(state_geometry(state), state_map(state), state_ui(state));
        draw_statusline(state);
        state_set_currkey(state, getch());
        input_parse(state_currkey(state));
    }

    cleanup();
    return 0;
}

