#include <ctype.h>
#include <math.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "include/draw.h"
#include "include/grid.h"
#include "include/interface.h"
#include "include/key.h"
#include "include/terrain.h"
#include "include/state.h"


int initialise(void)
{
    initscr();                  /* init the lib */
    cbreak();                   /* send by char (raw ignores ssigs! use cbreak) */
    noecho();                   /* don't echo user input */
    keypad(stdscr, TRUE);       /* capture special keys */
    intrflush(stdscr, FALSE);   /* interpret screen changes from ssigs correctly*/
    curs_set(0);                /* disable cursor */

    return 0;
}


void cleanup(void)
{
    erase();
    endwin();
}



enum INPUTMODE input_parse_capture(struct StateManager *s)
{
    if (ui_show(state_ui(s), PANEL_SPLASH)) {
        ui_toggle(state_ui(s), PANEL_SPLASH);
    }

    return INPUT_NAVIGATE;
}


enum INPUTMODE input_parse_navigate(struct StateManager *s, key ch)
{
    /* first handle the non-directional keys */
    switch (ch) {
        case KEY_TOGGLE_COMMAND:
            return INPUT_COMMAND;
        case KEY_TOGGLE_TERRAIN:
            ui_toggle(state_ui(s), PANEL_TERRAIN);
            return INPUT_TERRAIN;
        case KEY_AWAIT_TERRAIN:
            state_set_await(s, true);
            return INPUT_TERRAIN;
        case KEY_TOGGLE_DETAIL:
            ui_toggle(state_ui(s), PANEL_DETAIL);
            return INPUT_NAVIGATE;
        default:
            break;
    }

    if (key_is_direction(ch)) {
        enum DIRECTION d = key_direction(ch);
        int step_count = (key_is_special(ch)) ? 3 : 1;
        while (step_count) {
            map_step(state_map(s), d);
            step_count--;
        }
    }

    return INPUT_NAVIGATE;
}


enum INPUTMODE input_parse_terrain(struct StateManager *s, key ch)
{
    if (state_await(s)) {
        state_set_await(s, false);
        if (key_is_terrain(ch)) {
            map_paint(state_map(s), key_terrain(ch));
        }
        return INPUT_NAVIGATE;
    }
    /* first handle direct selection */

    if (key_is_terrain(ch)) {
        map_paint(state_map(s), key_terrain(ch));
        return INPUT_TERRAIN;
    }

    if (key_is_direction(ch)) {
        enum DIRECTION d = key_direction(ch);
        enum TERRAIN t = map_curr_terrain(state_map(s));
        map_step(state_map(s), d);

        if (key_is_special(ch)) {
            if (t != TERRAIN_UNKNOWN) {
                map_paint(state_map(s), t);
            }
        }
        return INPUT_TERRAIN;
    }

    switch (ch) {
        case KEY_TOGGLE_TERRAIN:
            ui_toggle(state_ui(s), PANEL_TERRAIN);
            return INPUT_NAVIGATE;
        default:
            break;
    }
    return INPUT_TERRAIN;
}


enum INPUTMODE input_parse_command(struct StateManager *s, key ch)
{
    if (ch == KEY_ENTER || ch == '\n') {
        char *buf = state_charbuf(s);
        if (buf[0] == KEY_TOGGLE_QUIT) {
            state_set_quit(s, true);
        }
        state_reset_charbuf(s);
        return INPUT_NAVIGATE;
    } else if (ch == KEY_BACKSPACE || ch == '\b' || ch == 127) {
        if (strlen(state_charbuf(s)) == 0) {
            state_reset_charbuf(s);
            return INPUT_NAVIGATE;
        }
        state_reset_nextchar(s);
    } else if (isprint(ch)) {
        state_set_nextchar(s, ch);
    }

    return INPUT_COMMAND;
}


void input_parse(struct StateManager *s, key ch)
{
    enum INPUTMODE next_mode = INPUT_NONE;

    switch (state_mode(s)) {
        case INPUT_CAPTURE:
            next_mode = input_parse_capture(s);
            break;
        case INPUT_NAVIGATE:
            next_mode = input_parse_navigate(s, ch);
            break;
        case INPUT_TERRAIN:
            next_mode = input_parse_terrain(s, ch);
            break;
        case INPUT_COMMAND:
            next_mode = input_parse_command(s, ch);
            break;
        default:
            break;
    }

    state_set_mode(s, next_mode);
}


int main(void)
{
    initialise();

    struct StateManager *s = state_create();
    state_initialise(s, stdscr);

    while (!state_quit(s)) {
        erase();
        draw_state(s);
        refresh();

        state_set_currkey(s, getch());
        input_parse(s, state_currkey(s));
        state_update(s);
    }

    state_destroy(s);

    cleanup();
    return 0;
}

