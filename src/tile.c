#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#include "hdr/tile.h"


static inline uint32_t xorshift(uint32_t x)
{
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    return x;
}


struct Tile
{
    struct Location *location;
    uint32_t seed;
    enum TERRAIN terrain;
    uint8_t roads;
    uint8_t rivers;
};


struct Tile *tile_create(void)
{
    static uint32_t global_seed = 0;
    if (global_seed == 0) global_seed = (uint32_t) time(NULL);
    global_seed = xorshift(global_seed);

    struct Tile *tile = malloc(sizeof(struct Tile));

    tile->seed = global_seed;
    tile->terrain = TERRAIN_UNKNOWN;
    tile->location = NULL;
    tile->roads = 0;
    tile->rivers = 0;

    return tile;
}


void tile_destroy(struct Tile *tile) { free(tile); }
uint32_t tile_seed(const struct Tile *tile) { return tile->seed; }
enum TERRAIN tile_terrain(const struct Tile *tile) { return tile->terrain; }
uint8_t tile_roads(const struct Tile *tile) { return tile->roads; }
uint8_t tile_rivers(const struct Tile *tile) { return tile->rivers; }
void tile_set_seed(struct Tile *tile, uint32_t seed) { tile->seed = seed; }
void tile_set_terrain(struct Tile *tile, enum TERRAIN t) { tile->terrain = t; }
void tile_set_roads(struct Tile *tile, uint8_t roads) { tile->roads = roads; }
void tile_set_rivers(struct Tile *tile, uint8_t rivers) { tile->rivers = rivers; }
void tile_clear_roads(struct Tile *tile) { tile->roads = 0; }
struct Location *tile_location(struct Tile *tile) { return tile->location; }
void tile_clear_rivers(struct Tile *tile) { tile->rivers = 0; }


bool tile_road(const struct Tile *tile, enum DIRECTION d)
{
    return tile->roads & (1 << d);
}


void tile_set_road(struct Tile *tile, enum DIRECTION d, bool b)
{
    if (b) tile->roads |= (1 << d);
    else tile->roads ^= tile->roads & (1 << d);
}


void tile_toggle_road(struct Tile *tile, enum DIRECTION d)
{
    tile->roads ^= (1 << d);
}


bool tile_river(const struct Tile *tile, enum DIRECTION d)
{
    return tile->rivers & (1 << d);
}


void tile_set_river(struct Tile *tile, enum DIRECTION d, bool b)
{
    if (b) tile->rivers |= (1 << d);
    else tile->rivers ^= tile->rivers & (1 << d);
}


void tile_toggle_river(struct Tile *tile, enum DIRECTION d)
{
    tile->rivers ^= (1 << d);
}


void tile_set_location(struct Tile *tile, struct Location *location)
{
    if (!tile) {
        return;
    }
    tile->location = location;
}


char tile_getch(struct Tile *tile, int x, int y)
{
    const char *chopts = terrain_chopts(tile_terrain(tile));
    uint32_t offset = tile->seed + tile_terrain(tile);
    uint32_t val = (x + xorshift(y + offset)) ^ (xorshift(x + offset) * y);
    return chopts[xorshift(val) % NUM_TERRAIN_CHOPTS];
}
