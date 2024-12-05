#ifndef TILE_H
#define TILE_H

#include <stdbool.h>

#include "coordinate.h"
#include "enum.h"

struct Tile;

struct Tile *tile_create();
void tile_destroy(struct Tile *t);
enum TERRAIN tile_terrain(const struct Tile *t);
void tile_set_terrain(struct Tile *tile, enum TERRAIN t);
bool tile_road(const struct Tile *t, enum DIRECTION d);
void tile_set_road(struct Tile *t, enum DIRECTION d, bool b);
void tile_toggle_road(struct Tile *tile, enum DIRECTION d);
void tile_clear_roads(struct Tile *tile);
bool tile_river(const struct Tile *t, enum DIRECTION d);
void tile_set_river(struct Tile *t, enum DIRECTION d, bool b);
void tile_toggle_river(struct Tile *tile, enum DIRECTION d);
void tile_clear_rivers(struct Tile *tile);
char tile_getch(struct Tile *tile, int x, int y);

#endif
