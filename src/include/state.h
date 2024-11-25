#ifndef STATE_H
#define STATE_H

#include <ncurses.h>

#include "key.h"

struct State;

struct State *state_create(void);
void state_initialise(struct State *s, WINDOW *win);
void state_update(struct State *s, key c);
void state_destroy(struct State *s);
struct Geometry *state_geometry(const struct State *s);
struct Map *state_map(const struct State *s);
struct UserInterface *state_ui(const struct State *s);
enum INPUT_MODE state_mode(const struct State *s);
void state_set_mode(struct State *s, enum INPUT_MODE mode);
char *state_charbuf(struct State *s);
void state_reset_charbuf(struct State *s);
void state_reset_nextchar(struct State *s);
void state_set_nextchar(struct State *s, char c);
enum UI_COLOUR state_colour(struct State *s);
void state_set_colour(struct State *s, enum UI_COLOUR colour);
bool state_await(struct State *s);
void state_set_await(struct State *s, bool await);
bool state_quit(struct State *s);
void state_set_quit(struct State *s, bool quit);
key state_currkey(struct State *s);
void state_set_currkey(struct State *s, key k);
void state_set_mode(struct State *s, enum INPUT_MODE mode);
int state_mode_colour(const struct State *s);
const char *state_mode_name(const struct State *s);

#endif
