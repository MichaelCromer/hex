#ifndef STATE_H
#define STATE_H

#include <ncurses.h>

#include "key.h"

struct StateManager;

struct StateManager *state_create(void);
void state_initialise(struct StateManager *sm, WINDOW *win);
void state_update(struct StateManager *sm);
void state_destroy(struct StateManager *s);
struct Geometry *state_geometry(const struct StateManager *sm);
struct Map *state_map(const struct StateManager *sm);
struct UserInterface *state_ui(const struct StateManager *sm);
enum INPUTMODE state_mode(const struct StateManager *sm);
void state_set_mode(struct StateManager *sm, enum INPUTMODE mode);
char *state_charbuf(struct StateManager *sm);
void state_reset_charbuf(struct StateManager *sm);
void state_reset_nextchar(struct StateManager *sm);
void state_set_nextchar(struct StateManager *sm, char c);
enum UI_COLOUR state_colour(struct StateManager *sm);
void state_set_colour(struct StateManager *sm, enum UI_COLOUR colour);
bool state_await(struct StateManager *sm);
void state_set_await(struct StateManager *sm, bool await);
bool state_quit(struct StateManager *sm);
void state_set_quit(struct StateManager *sm, bool quit);
key state_currkey(struct StateManager *sm);
void state_set_currkey(struct StateManager *sm, key k);
void state_set_mode(struct StateManager *sm, enum INPUTMODE mode);
int state_mode_colour(const struct StateManager *sm);
const char *state_mode_name(const struct StateManager *sm);

#endif
