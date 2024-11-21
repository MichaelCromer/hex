#include <math.h>
#include <ncurses.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "include/enum.h"
#include "include/geometry.h"
#include "include/grid.h"
#include "include/interface.h"
#include "include/key.h"
#include "include/panel.h"
#include "include/state.h"
#include "include/terrain.h"

#define STATE_CHARBUF_LEN 1024

const char *modestr_navigate = "NAV";
const char *modestr_terrain = "TRN";
const char *modestr_command = "CMD";
const char *modestr_unknown = "???";


struct StateManager {
    bool quit;
    bool await;
    bool reticule;

    char charbuf[STATE_CHARBUF_LEN];
    char *nextchar;

    key currkey;
    enum INPUTMODE mode;

    WINDOW *win;
    enum UI_COLOUR colour;

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
    sm->currkey = 0;
    memset(sm->charbuf, 0, STATE_CHARBUF_LEN);
    sm->nextchar = sm->charbuf;
    sm->mode = INPUT_NONE;
    sm->colour = COLOUR_NONE;
    sm->win = NULL;

    sm->geometry = geometry_create();
    sm->ui = ui_create();
    sm->map = map_create();

    return sm;
}


void state_initialise(struct StateManager *sm, WINDOW *win)
{
    sm->win = win;
    state_set_mode(sm, INPUT_CAPTURE);

    /* set up colour */
    enum UI_COLOUR colour = (has_colors() == TRUE)
        ? ( (can_change_color() == TRUE) ? COLOUR_MANY : COLOUR_SOME)
        : COLOUR_NONE;
    state_set_colour(sm, colour);

    if (state_colour(sm) == COLOUR_SOME || state_colour(sm) == COLOUR_MANY) {
        start_color();
        init_pair(1, COLOR_RED, COLOR_BLACK);
        init_pair(2, COLOR_GREEN, COLOR_BLACK);
        init_pair(3, COLOR_YELLOW, COLOR_BLACK);
        init_pair(4, COLOR_BLUE, COLOR_BLACK);
        init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
        init_pair(6, COLOR_CYAN, COLOR_BLACK);
        init_pair(7, COLOR_WHITE, COLOR_BLACK);
    }

    geometry_initialise(
            state_geometry(sm),
            GEOMETRY_DEFAULT_SCALE,
            GEOMETRY_DEFAULT_ASPECT,
            win);
    ui_initialise(state_ui(sm), state_geometry(sm));
    map_initialise(state_map(sm), hex_origin());

}


void state_update(struct StateManager *sm)
{
    struct Hex *current_hex = map_curr(state_map(sm));

    /* TODO this is ridiculous as written here */
    struct Panel *hex_detail = ui_panel(state_ui(sm), PANEL_DETAIL);
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


enum INPUTMODE state_mode(const struct StateManager *sm)
{
    return sm->mode;
}


void state_set_mode(struct StateManager *sm, enum INPUTMODE mode)
{
    sm->mode = mode;
}



key state_currkey(struct StateManager *sm)
{
    return sm->currkey;
}


void state_set_currkey(struct StateManager *sm, key k)
{
    sm->currkey = k;
}


bool state_quit(struct StateManager *sm)
{
    return sm->quit;
}


void state_set_quit(struct StateManager *sm, bool quit)
{
    sm->quit = quit;
}


enum UI_COLOUR state_colour(struct StateManager *sm)
{
    return sm->colour;
}


void state_set_colour(struct StateManager *sm, enum UI_COLOUR colour)
{
    sm->colour = colour;
}


bool state_await(struct StateManager *sm)
{
    return sm->await;
}


void state_set_await(struct StateManager *sm, bool await)
{
    sm->await = await;
}


char *state_charbuf(struct StateManager *sm)
{
    return sm->charbuf;
}


void state_reset_charbuf(struct StateManager *sm)
{
    memset(sm->charbuf, 0, STATE_CHARBUF_LEN);
    sm->nextchar = sm->charbuf;
}


void state_reset_nextchar(struct StateManager *sm)
{
    if (sm->nextchar == sm->charbuf) {
        return;
    }
    sm->nextchar--;
    *(sm->nextchar) = 0;
}


void state_set_nextchar(struct StateManager *sm, char c)
{
    if (sm->nextchar - sm->charbuf >= STATE_CHARBUF_LEN) {
        return;
    }
    *(sm->nextchar) = c;
    sm->nextchar++;
}


enum UI_COLOUR state_colour_test(void)
{
    return (has_colors())
        ? ((can_change_color()) ? COLOUR_MANY : COLOUR_SOME)
        : COLOUR_NONE;
}


const char *state_mode_name(const struct StateManager *sm)
{
    switch (state_mode(sm)) {
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

int state_mode_colour(const struct StateManager *sm)
{
    switch (state_mode(sm)) {
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
