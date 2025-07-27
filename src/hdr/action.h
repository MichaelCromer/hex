#ifndef ACTION_H
#define ACTION_H

#include "enum.h"
#include "atlas.h"
#include "state.h"

void action_edit(const char *filename);
void action_hint(void);
void action_capture(key k);
void action_navigate(key k);
void action_terrain(key k);
void action_command(key k);
void action_road(key k);
void action_river(key k);
void action_location(key k);

#endif
