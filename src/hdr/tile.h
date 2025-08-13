#ifndef TILE_H
#define TILE_H

#include <stdbool.h>

#include "coordinate.h"
#include "enum.h"
#include "location.h"

struct Tile;

struct Tile *tile_create(void);
void tile_destroy(struct Tile *tile);
unsigned int tile_seed(const struct Tile *tile);
uint8_t tile_roads(const struct Tile *tile);
uint8_t tile_rivers(const struct Tile *tile);
void tile_set_roads(struct Tile *tile, uint8_t roads);
void tile_set_rivers(struct Tile *tile, uint8_t rivers);
void tile_set_seed(struct Tile *tile, unsigned int seed);
enum TERRAIN tile_terrain(const struct Tile *tile);
void tile_set_terrain(struct Tile *tile, enum TERRAIN t);
bool tile_road(const struct Tile *tile, enum DIRECTION d);
void tile_set_road(struct Tile *tile, enum DIRECTION d, bool b);
void tile_toggle_road(struct Tile *tile, enum DIRECTION d);
void tile_clear_roads(struct Tile *tile);
bool tile_river(const struct Tile *tile, enum DIRECTION d);
void tile_set_river(struct Tile *tile, enum DIRECTION d, bool b);
void tile_toggle_river(struct Tile *tile, enum DIRECTION d);
void tile_clear_rivers(struct Tile *tile);
struct Location *tile_location(struct Tile *tile);
void tile_set_location(struct Tile *tile, struct Location *location);
char tile_getch(struct Tile *tile, int x, int y);

#endif
