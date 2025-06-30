#ifndef STATE_H
#define STATE_H

#include <ncurses.h>

#include "key.h"

struct State;

key state_key_curr(void);
bool state_quit(void);
void state_initialise(WINDOW * win);
void state_update(void);
void state_deinitialise(void);
struct Atlas *state_atlas(void);
void state_set_atlas(struct Atlas *atlas);
void state_clear_atlas(void);
enum MODE state_mode(void);
enum MODE state_lastmode(void);
void state_push_mode(enum MODE mode);
void state_pop_mode(void);
bool state_await(void);
WINDOW *state_window(void);
void state_set_quit(bool quit);

#endif
