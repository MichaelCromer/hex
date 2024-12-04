#include <math.h>
#include <ncurses.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "include/action.h"
#include "include/commandline.h"
#include "include/enum.h"
#include "include/geometry.h"
#include "include/atlas.h"
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

    struct Geometry *geometry;
    struct Atlas *atlas;
    struct UserInterface *ui;
    struct Commandline *cmd;
};


struct State *state_create(void)
{
    struct State *s = malloc(sizeof(struct State));

    s->quit = false;
    s->reticule = false;
    s->currkey = 0;
    s->mode = MODE_NONE;
    s->colour = COLOUR_NONE;
    s->win = NULL;

    s->geometry = geometry_create();
    s->ui = ui_create();
    s->atlas = atlas_create();
    s->cmd = commandline_create();

    return s;
}


void state_initialise(struct State *s, WINDOW *win)
{
    s->win = win;
    state_push_mode(s, MODE_CAPTURE);

    /* set up colour */
    enum UI_COLOUR colour = (has_colors() == TRUE)
        ? ( (can_change_color() == TRUE) ? COLOUR_MANY : COLOUR_SOME)
        : COLOUR_NONE;
    state_set_colour(s, colour);

    if (state_colour(s) == COLOUR_SOME || state_colour(s) == COLOUR_MANY) {
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
            state_geometry(s),
            GEOMETRY_DEFAULT_SCALE,
            GEOMETRY_DEFAULT_ASPECT,
            win);
    ui_initialise(state_ui(s), state_geometry(s));
    atlas_initialise(state_atlas(s), chart_create_origin());
}


void state_update(struct State *s)
{
    key k = wgetch(state_window(s));
    state_set_currkey(s, k);

    switch (state_mode(s)) {
        case MODE_CAPTURE:
            action_capture(s, k);
            break;
        case MODE_NAVIGATE:
            action_navigate(s, k);
            break;
        case MODE_TERRAIN:
        case MODE_AWAIT_TERRAIN:
            action_terrain(s, k);
            break;
        case MODE_COMMAND:
            action_command(s, k);
            break;
        case MODE_ROAD:
        case MODE_AWAIT_ROAD:
            action_road(s, k);
            break;
        case MODE_AWAIT_RIVER:
        case MODE_RIVER:
            action_river(s, k);
            break;
        case MODE_NONE:
        default:
            break;
    }

    return;
}


void state_destroy(struct State *s)
{
    geometry_destroy(s->geometry);
    atlas_destroy(s->atlas);
    ui_destroy(s->ui);
    commandline_destroy(s->cmd);

    free(s);
    s = NULL;
    return;
}


struct Geometry *state_geometry(const struct State *s)
{
    return s->geometry;
}


struct Atlas *state_atlas(const struct State *s)
{
    return s->atlas;
}


struct UserInterface *state_ui(const struct State *s)
{
    return s->ui;
}


struct Commandline *state_commandline(const struct State *s)
{
    return s->cmd;
}


enum MODE state_mode(const struct State *s)
{
    return s->mode;
}


enum MODE state_lastmode(const struct State *state)
{
    return state->lastmode;
}


void state_push_mode(struct State *s, enum MODE mode)
{
    s->lastmode = s->mode;
    s->mode = mode;
}


void state_pop_mode(struct State *state)
{
    state->mode = state->lastmode;
}


key state_currkey(struct State *s)
{
    return s->currkey;
}


void state_set_currkey(struct State *s, key k)
{
    s->currkey = k;
}


bool state_quit(struct State *s)
{
    return s->quit;
}


void state_set_quit(struct State *s, bool quit)
{
    s->quit = quit;
}


enum UI_COLOUR state_colour(struct State *s)
{
    return s->colour;
}


void state_set_colour(struct State *s, enum UI_COLOUR colour)
{
    s->colour = colour;
}


bool state_await(struct State *s)
{
    return mode_is_await(state_mode(s));
}


WINDOW *state_window(struct State *s)
{
    return s->win;
}


enum UI_COLOUR state_colour_test(void)
{
    return (has_colors())
        ? ((can_change_color()) ? COLOUR_MANY : COLOUR_SOME)
        : COLOUR_NONE;
}

