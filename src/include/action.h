#ifndef ACTION_H
#define ACTION_H

#include "enum.h"
#include "atlas.h"
#include "state.h"

void action_capture(struct State *state, key k);
void action_navigate(struct State *state, key k);
void action_terrain(struct State *state, key k);
void action_command(struct State *state, key k);
void action_road(struct State *state, key k);
void action_river(struct State *state, key k);
void action_feature(struct State *state, key k);

#endif
