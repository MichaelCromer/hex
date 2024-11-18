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

struct StateManager {
    bool quit;
    bool await;
    bool reticule;
    char cmdbuf[1024];
    key lastch;
    enum INPUTMODE input_mode;
    enum UI_COLOUR colours;

    struct Geometry *geometry;
    struct Map *map;
    struct UserInterface *ui;
};


struct StateManager *state_create(void)
{
    struct StateManager *sm = malloc(sizeof(struct StateManager));

    sm->quit = false;
    sm->await = false;
    sm->reticule = false;
    sm->lastch = 0;
    memset(sm->cmdbuf, 0, 1024);
    sm->input_mode = INPUT_NONE;
    sm->colours = COLOUR_NONE;

    sm->geometry = NULL;
    sm->map = NULL;
    sm->ui = NULL;

    return sm;
}


void state_destroy(struct StateManager *s)
{
    geometry_destroy(s->geometry);
    map_destroy(s->map);
    ui_destroy(s->ui);

    free(s);
    s = NULL;
    return;
}


struct Geometry *state_geometry(const struct StateManager *sm)
{
    return sm->geometry;
}


struct Map *state_map(const struct StateManager *sm)
{
    return sm->map;
}


struct UserInterface *state_ui(const struct StateManager *sm)
{
    return sm->ui;
}


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


void draw_statusline(struct StateManager *s)
{
    int r0 = geometry_rows(state_geometry(s))-1,
        c0 = 0,
        w  = geometry_cols(state_geometry(s))-1;

    mvhline(r0, c0, ' ', w);
    attron(COLOR_PAIR(mode_colour(s->input_mode)));
    mvaddstr(r0, c0+1, modestr(s->input_mode));
    attroff(COLOR_PAIR(mode_colour(s->input_mode)));
    if (s->input_mode == INPUT_COMMAND) {
        addch(' ');
        addch(':');
        addstr(s->cmdbuf);
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
    state->input_mode = INPUT_CAPTURE;

    /* set up colours */
    state->colours = (has_colors())
        ? ((can_change_color()) ? COLOUR_MANY : COLOUR_SOME)
        : COLOUR_NONE;

    if (state->colours == COLOUR_SOME || state->colours == COLOUR_MANY) {
        start_color();
        init_pair(1, COLOR_RED, COLOR_BLACK);
        init_pair(2, COLOR_GREEN, COLOR_BLACK);
        init_pair(3, COLOR_YELLOW, COLOR_BLACK);
        init_pair(4, COLOR_BLUE, COLOR_BLACK);
        init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
        init_pair(6, COLOR_CYAN, COLOR_BLACK);
        init_pair(7, COLOR_WHITE, COLOR_BLACK);
    }


    state->geometry = geometry_create(
            GEOMETRY_DEFAULT_SCALE,
            GEOMETRY_DEFAULT_ASPECT,
            stdscr
            );

    state->ui = ui_create();
    ui_initialise(state_ui(state), state_geometry(state));

    state->map = map_create(hex_origin());
    map_goto(state_map(state), coordinate_zero());

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
            state->await=true;
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
    if (state->await) {
        state->await = false;
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
        if (state->cmdbuf[0] == KEY_TOGGLE_QUIT) {
            state->quit = true;
        }
        memset(state->cmdbuf, 0, 1024);
        update_vars();
        return INPUT_NAVIGATE;
    } else if (ch == KEY_BACKSPACE || ch == '\b' || ch == 127) {
        int L = strlen(state->cmdbuf);
        if (L <= 0) {
            memset(state->cmdbuf, 0, 1024);
            update_vars();
            return INPUT_NAVIGATE;
        }
        state->cmdbuf[L-1] = 0;
        update_vars();
    } else if (isprint(ch)) {
        int L = strlen(state->cmdbuf);
        state->cmdbuf[L] = ch;
    }

    return INPUT_COMMAND;
}


void input_parse(key ch)
{
    enum INPUTMODE next_mode = INPUT_NONE;

    switch (state->input_mode) {
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

    state->input_mode = next_mode;
    update_vars();
}


int main(void)
{
    initialise();

    while (!state->quit) {
        draw_screen(state_geometry(state), state_map(state), state_ui(state));
        draw_statusline(state);
        state->lastch = getch();
        input_parse(state->lastch);
    }

    cleanup();
    return 0;
}

