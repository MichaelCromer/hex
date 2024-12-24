#ifndef STATE_H
#define STATE_H

#include <ncurses.h>

#include "key.h"

struct State;

struct State *state_create(void);
void state_initialise(struct State *state, WINDOW * win);
void state_update(struct State *state);
void state_destroy(struct State *state);
struct Geometry *state_geometry(const struct State *state);
struct Atlas *state_atlas(const struct State *state);
void state_set_atlas(struct State *state, struct Atlas *atlas);
void state_clear_atlas(struct State *state);
struct UserInterface *state_ui(const struct State *state);
struct Commandline *state_commandline(const struct State *state);
enum MODE state_mode(const struct State *state);
enum MODE state_lastmode(const struct State *state);
void state_push_mode(struct State *state, enum MODE mode);
void state_pop_mode(struct State *state);
enum UI_COLOUR state_colour(struct State *state);
bool state_await(struct State *state);
WINDOW *state_window(struct State *state);
bool state_quit(struct State *state);
void state_set_quit(struct State *state, bool quit);
key state_currkey(struct State *state);

#endif
