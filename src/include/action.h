#ifndef ACTION_H
#define ACTION_H

#include "enum.h"
#include "atlas.h"
#include "state.h"

void action_capture(struct State *s, key k);
void action_navigate(struct State *s, key k);
void action_terrain(struct State *s, key k);
void action_command(struct State *s, key k);
void action_road(struct State *s, key k);
void action_river(struct State *s, key k);

#endif
