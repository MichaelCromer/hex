#include <math.h>
#include <ncurses.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "include/action.h"
#include "include/atlas.h"
#include "include/commandline.h"
#include "include/enum.h"
#include "include/geometry.h"
#include "include/interface.h"
#include "include/key.h"
#include "include/panel.h"
#include "include/state.h"

struct State {
    bool quit;
    bool reticule;

    key currkey;
    enum MODE mode;
    enum MODE lastmode;

    WINDOW *win;
    enum UI_COLOUR colour;

    struct Atlas *atlas;
    struct Commandline *cmd;
    struct Geometry *geometry;
    struct UserInterface *ui;
};

struct State *state_create(void)
{
    struct State *state = malloc(sizeof(struct State));

    state->quit = false;
    state->reticule = false;
    state->currkey = 0;
    state->mode = MODE_NONE;
    state->colour = COLOUR_NONE;
    state->win = NULL;

    state->atlas = atlas_create();
    state->cmd = commandline_create();
    state->geometry = geometry_create();
    state->ui = ui_create();

    return state;
}

void state_initialise(struct State *state, WINDOW *win)
{
    state->win = win;
    state_push_mode(state, MODE_CAPTURE);

    state->colour = colour_test();
    if (state_colour(state) == COLOUR_SOME || state_colour(state) == COLOUR_MANY) {
        start_color();
        init_pair(1, COLOR_RED, COLOR_BLACK);
        init_pair(2, COLOR_GREEN, COLOR_BLACK);
        init_pair(3, COLOR_YELLOW, COLOR_BLACK);
        init_pair(4, COLOR_BLUE, COLOR_BLACK);
        init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
        init_pair(6, COLOR_CYAN, COLOR_BLACK);
        init_pair(7, COLOR_WHITE, COLOR_BLACK);
    }

    geometry_initialise(state_geometry(state),
                        GEOMETRY_DEFAULT_SCALE, GEOMETRY_DEFAULT_ASPECT, win);
    ui_initialise(state_ui(state), state_geometry(state));
    atlas_initialise(state_atlas(state), state_geometry(state));
}

void state_update(struct State *state)
{
    key k = wgetch(state_window(state));
    state->currkey = k;

    switch (state_mode(state)) {
        case MODE_CAPTURE:
            action_capture(state, k);
            break;
        case MODE_NAVIGATE:
            action_navigate(state, k);
            break;
        case MODE_TERRAIN:
        case MODE_AWAIT_TERRAIN:
            action_terrain(state, k);
            break;
        case MODE_COMMAND:
            action_command(state, k);
            break;
        case MODE_ROAD:
        case MODE_AWAIT_ROAD:
            action_road(state, k);
            break;
        case MODE_AWAIT_RIVER:
        case MODE_RIVER:
            action_river(state, k);
            break;
        case MODE_AWAIT_LOCATION:
        case MODE_LOCATION:
            action_location(state, k);
            break;
        case MODE_NONE:
        default:
            break;
    }

    return;
}

void state_destroy(struct State *state)
{
    atlas_destroy(state->atlas);
    commandline_destroy(state->cmd);
    geometry_destroy(state->geometry);
    ui_destroy(state->ui);

    free(state);
    state = NULL;
    return;
}

struct Geometry *state_geometry(const struct State *state)
{
    return state->geometry;
}

struct Atlas *state_atlas(const struct State *state)
{
    return state->atlas;
}

void state_set_atlas(struct State *state, struct Atlas *atlas)
{
    if (state_atlas(state)) {
        return;
    }
    state->atlas = atlas;
}

void state_clear_atlas(struct State *state)
{
    if (!state_atlas(state)) {
        return;
    }
    atlas_destroy(state_atlas(state));
    state->atlas = NULL;
}

struct UserInterface *state_ui(const struct State *state)
{
    return state->ui;
}

struct Commandline *state_commandline(const struct State *state)
{
    return state->cmd;
}

enum MODE state_mode(const struct State *state)
{
    return state->mode;
}

enum MODE state_lastmode(const struct State *state)
{
    return state->lastmode;
}

void state_push_mode(struct State *state, enum MODE mode)
{
    state->lastmode = state->mode;
    state->mode = mode;
}

void state_pop_mode(struct State *state)
{
    state->mode = state->lastmode;
}

key state_currkey(struct State *state)
{
    return state->currkey;
}

bool state_quit(struct State *state)
{
    return state->quit;
}

void state_set_quit(struct State *state, bool quit)
{
    state->quit = quit;
}

enum UI_COLOUR state_colour(struct State *state)
{
    return state->colour;
}

bool state_await(struct State *state)
{
    return mode_is_await(state_mode(state));
}

WINDOW *state_window(struct State *state)
{
    return state->win;
}
