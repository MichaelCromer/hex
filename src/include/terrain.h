#ifndef TERRAIN_H
#define TERRAIN_H

#include "enum.h"

const char *terrain_name(enum TERRAIN t);
char terrain_getch(enum TERRAIN t, int x, int y, int seed);
int terrain_colour(enum TERRAIN t);
const char *terrain_statusline(void);

#endif
