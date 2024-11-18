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
};


struct StateManager *state_create(void)
{
    struct StateManager *s = malloc(sizeof(struct StateManager));

    s->quit = false;
    s->await = false;
    s->reticule = false;
    s->lastch = 0;
    memset(s->cmdbuf, 0, 1024);
    s->input_mode = INPUT_NONE;
    s->colours = COLOUR_NONE;

    s->geometry = NULL;

    return s;
}


void state_destroy(struct StateManager *s)
{
    geometry_destroy(s->geometry);

    free(s);
    s = NULL;
    return;
}


struct Geometry *state_geometry(const struct StateManager *sm)
{
    return sm->geometry;
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

struct Map *map = NULL;
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

    sm = state_create();
    sm->input_mode = INPUT_CAPTURE;

    /* set up colours */
    sm->colours = (has_colors())
        ? ((can_change_color()) ? COLOUR_MANY : COLOUR_SOME)
        : COLOUR_NONE;

    if (sm->colours == COLOUR_SOME || sm->colours == COLOUR_MANY) {
        start_color();
        init_pair(1, COLOR_RED, COLOR_BLACK);
        init_pair(2, COLOR_GREEN, COLOR_BLACK);
        init_pair(3, COLOR_YELLOW, COLOR_BLACK);
        init_pair(4, COLOR_BLUE, COLOR_BLACK);
        init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
        init_pair(6, COLOR_CYAN, COLOR_BLACK);
        init_pair(7, COLOR_WHITE, COLOR_BLACK);
    }


    sm->geometry = geometry_create(
            GEOMETRY_DEFAULT_SCALE,
            GEOMETRY_DEFAULT_ASPECT,
            stdscr
            );

    ui = ui_create();
    ui_initialise(ui, state_geometry(sm));

    map = map_create(hex_origin());
    map_goto(map, coordinate_zero());

    update_vars();

    return 0;
}


void cleanup(void)
{
    ui_destroy(ui);
    state_destroy(sm);
    map_destroy(map);

    erase();
    endwin();
}



enum INPUTMODE input_parse_capture(void)
{
    if (ui_show(ui, PANEL_SPLASH)) {
        ui_toggle(ui, PANEL_SPLASH);
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
            ui_toggle(ui, PANEL_TERRAIN);
            return INPUT_TERRAIN;
        case KEY_AWAIT_TERRAIN:
            sm->await=true;
            return INPUT_TERRAIN;
        case KEY_TOGGLE_DETAIL:
            ui_toggle(ui, PANEL_DETAIL);
            return INPUT_NAVIGATE;
        default:
            break;
    }

    if (key_is_direction(ch)) {
        enum DIRECTION d = key_direction(ch);
        int step_count = (key_is_special(ch)) ? 3 : 1;
        while (step_count) {
            map_step(map, d);
            step_count--;
        }
    }

    return INPUT_NAVIGATE;
}


enum INPUTMODE input_parse_terrain(key ch)
{
    if (sm->await) {
        sm->await = false;
        if (key_is_terrain(ch)) {
            map_paint(map, key_terrain(ch));
        }
        return INPUT_NAVIGATE;
    }
    /* first handle direct selection */

    if (key_is_terrain(ch)) {
        map_paint(map, key_terrain(ch));
        return INPUT_TERRAIN;
    }

    if (key_is_direction(ch)) {
        enum DIRECTION d = key_direction(ch);
        enum TERRAIN t = map_curr_terrain(map);
        map_step(map, d);

        if (key_is_special(ch)) {
            if (t != TERRAIN_UNKNOWN) {
                map_paint(map, t);
            }
        }
        return INPUT_TERRAIN;
    }

    switch (ch) {
        case KEY_TOGGLE_TERRAIN:
            ui_toggle(ui, PANEL_TERRAIN);
            return INPUT_NAVIGATE;
        default:
            break;
    }
    return INPUT_TERRAIN;
}


enum INPUTMODE input_parse_command(key ch)
{
    if (ch == KEY_ENTER || ch == '\n') {
        if (sm->cmdbuf[0] == KEY_TOGGLE_QUIT) {
            sm->quit = true;
        }
        memset(sm->cmdbuf, 0, 1024);
        update_vars();
        return INPUT_NAVIGATE;
    } else if (ch == KEY_BACKSPACE || ch == '\b' || ch == 127) {
        int L = strlen(sm->cmdbuf);
        if (L <= 0) {
            memset(sm->cmdbuf, 0, 1024);
            update_vars();
            return INPUT_NAVIGATE;
        }
        sm->cmdbuf[L-1] = 0;
        update_vars();
    } else if (isprint(ch)) {
        int L = strlen(sm->cmdbuf);
        sm->cmdbuf[L] = ch;
    }

    return INPUT_COMMAND;
}


void input_parse(key ch)
{
    enum INPUTMODE next_mode = INPUT_NONE;

    switch (sm->input_mode) {
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

    sm->input_mode = next_mode;
    update_vars();
}


int main(void)
{
    initialise();

    while (!sm->quit) {
        draw_screen(state_geometry(sm), map, ui);
        draw_statusline(sm);
        sm->lastch = getch();
        input_parse(sm->lastch);
    }

    cleanup();
    return 0;
}

