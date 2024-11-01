#ifndef TERRAIN_H
#define TERRAIN_H

#include "enum.h"

const char *terrain_name(enum TERRAIN t);
char terrain_getch(enum TERRAIN t, int x, int y, int seed);

#endif
