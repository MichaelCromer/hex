#ifndef STATE_H
#define STATE_H

#include <ncurses.h>

#include "key.h"

struct State;

struct State *state_create(void);
void state_initialise(struct State *s, WINDOW *win);
void state_update(struct State *s);
void state_destroy(struct State *s);
struct Geometry *state_geometry(const struct State *s);
struct Atlas *state_atlas(const struct State *s);
struct UserInterface *state_ui(const struct State *s);
struct Commandline *state_commandline(const struct State *s);
enum MODE state_mode(const struct State *s);
enum MODE state_lastmode(const struct State *state);
void state_push_mode(struct State *s, enum MODE mode);
void state_pop_mode(struct State *state);
enum UI_COLOUR state_colour(struct State *s);
bool state_await(struct State *s);
WINDOW *state_window(struct State *s);
bool state_quit(struct State *s);
void state_set_quit(struct State *s, bool quit);
key state_currkey(struct State *s);

#endif
