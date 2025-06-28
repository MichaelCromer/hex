#include <math.h>
#include <ncurses.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "hdr/action.h"
#include "hdr/atlas.h"
#include "hdr/enum.h"
#include "hdr/geometry.h"
#include "hdr/interface.h"
#include "hdr/key.h"
#include "hdr/state.h"

struct State {
    bool quit;
    bool reticule;

    key currkey;
    enum MODE mode;
    enum MODE lastmode;

    WINDOW *win;

    struct Atlas *atlas;
    struct Commandline *cmd;
    struct UserInterface *ui;
};


struct State *state_create(void)
{
    struct State *state = malloc(sizeof(struct State));

    state->quit = false;
    state->reticule = false;
    state->currkey = 0;
    state->mode = MODE_NONE;
    state->win = NULL;

    state->atlas = atlas_create();

    return state;
}

void state_initialise(struct State *state, WINDOW *win)
{
    state->win = win;
    state_push_mode(state, MODE_CAPTURE);

    geometry_initialise(GEOMETRY_DEFAULT_SCALE, GEOMETRY_DEFAULT_ASPECT, win);
    ui_initialise();
    atlas_initialise(state_atlas(state));
}

void state_update(struct State *state)
{
    key k = wgetch(state_window(state));
    state->currkey = k;

    if ((MODE_COMMAND != state_mode(state)) && (k == '?')) {
        action_hint(state);
        return;
    }

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

    ui_update(state);
    geometry_calculate_viewpoint(atlas_coordinate(state_atlas(state)));
}

void state_destroy(struct State *state)
{
    atlas_destroy(state->atlas);

    free(state);
    state = NULL;
    return;
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

bool state_await(struct State *state)
{
    return mode_is_await(state_mode(state));
}

WINDOW *state_window(struct State *state)
{
    return state->win;
}
