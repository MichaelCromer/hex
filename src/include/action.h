#ifndef ACTION_H
#define ACTION_H

#include "enum.h"
#include "grid.h"
#include "state.h"

void action_move(struct State *s, enum DIRECTION d, int steps);
void action_paint(struct State *s, enum TERRAIN t);
void action_drag(struct State *s, enum DIRECTION d);

#endif
