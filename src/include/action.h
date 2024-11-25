#ifndef ACTION_H
#define ACTION_H

#include "enum.h"
#include "grid.h"
#include "state.h"

void action_move(struct Map *m, enum DIRECTION d, int steps);
void action_paint(struct Map *m, enum TERRAIN t);
void action_capture(struct State *s, key k);
void action_navigate(struct State *s, key k);
void action_terrain(struct State *s, key k);
void action_command(struct State *s, key k);

#endif
