#include <math.h>
#include <ncurses.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "include/action.h"
#include "include/enum.h"
#include "include/geometry.h"
#include "include/grid.h"
#include "include/interface.h"
#include "include/key.h"
#include "include/panel.h"
#include "include/state.h"
#include "include/terrain.h"

#define STATE_CHARBUF_LEN 1024

const char *modestr_navigate = "NAVIGATE";
const char *modestr_terrain = "TERRAIN";
const char *modestr_command = "COMMAND";
const char *modestr_unknown = "???";


struct State {
    bool quit;
    bool await;
    bool reticule;

    char charbuf[STATE_CHARBUF_LEN];
    char *nextchar;

    key currkey;
    enum INPUT_MODE mode;

    WINDOW *win;
    enum UI_COLOUR colour;

    struct Geometry *geometry;
    struct Map *map;
    struct UserInterface *ui;
};


struct State *state_create(void)
{
    struct State *s = malloc(sizeof(struct State));

    s->quit = false;
    s->await = false;
    s->reticule = false;
    s->currkey = 0;
    memset(s->charbuf, 0, STATE_CHARBUF_LEN);
    s->nextchar = s->charbuf;
    s->mode = INPUT_MODE_NONE;
    s->colour = COLOUR_NONE;
    s->win = NULL;

    s->geometry = geometry_create();
    s->ui = ui_create();
    s->map = map_create();

    return s;
}


void state_initialise(struct State *s, WINDOW *win)
{
    s->win = win;
    state_set_mode(s, INPUT_MODE_CAPTURE);

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
    map_initialise(state_map(s), hex_origin());

}


void state_update(struct State *s, key c)
{
    switch (state_mode(s)) {
        case INPUT_MODE_CAPTURE:
            action_capture(s, c);
            break;
        case INPUT_MODE_NAVIGATE:
            action_navigate(s, c);
            break;
        case INPUT_MODE_TERRAIN:
            action_terrain(s, c);
            break;
        case INPUT_MODE_COMMAND:
            action_command(s, c);
            break;
        case INPUT_MODE_NONE:
        default:
            break;
    }

    return;
}


void state_destroy(struct State *s)
{
    geometry_destroy(s->geometry);
    map_destroy(s->map);
    ui_destroy(s->ui);

    free(s);
    s = NULL;
    return;
}


struct Geometry *state_geometry(const struct State *s)
{
    return s->geometry;
}


struct Map *state_map(const struct State *s)
{
    return s->map;
}


struct UserInterface *state_ui(const struct State *s)
{
    return s->ui;
}


enum INPUT_MODE state_mode(const struct State *s)
{
    return s->mode;
}


void state_set_mode(struct State *s, enum INPUT_MODE mode)
{
    s->mode = mode;
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
    return s->await;
}


void state_set_await(struct State *s, bool await)
{
    s->await = await;
}


char *state_charbuf(struct State *s)
{
    return s->charbuf;
}


void state_reset_charbuf(struct State *s)
{
    memset(s->charbuf, 0, STATE_CHARBUF_LEN);
    s->nextchar = s->charbuf;
}


void state_reset_nextchar(struct State *s)
{
    if (s->nextchar == s->charbuf) {
        return;
    }
    s->nextchar--;
    *(s->nextchar) = 0;
}


void state_set_nextchar(struct State *s, char c)
{
    if (s->nextchar - s->charbuf >= STATE_CHARBUF_LEN) {
        return;
    }
    *(s->nextchar) = c;
    s->nextchar++;
}


enum UI_COLOUR state_colour_test(void)
{
    return (has_colors())
        ? ((can_change_color()) ? COLOUR_MANY : COLOUR_SOME)
        : COLOUR_NONE;
}


const char *state_mode_name(const struct State *s)
{
    switch (state_mode(s)) {
        case INPUT_MODE_NAVIGATE:
            return modestr_navigate;
        case INPUT_MODE_TERRAIN:
            return modestr_terrain;
        case INPUT_MODE_COMMAND:
            return modestr_command;
        default:
            return modestr_unknown;
    }
}

int state_mode_colour(const struct State *s)
{
    switch (state_mode(s)) {
        case INPUT_MODE_NAVIGATE:
            return COLOR_WHITE;
        case INPUT_MODE_TERRAIN:
            return COLOR_GREEN;
        case INPUT_MODE_COMMAND:
            return COLOR_RED;
        default:
            return COLOR_WHITE;
    }
}
