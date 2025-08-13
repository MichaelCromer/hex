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
    unsigned int seed;
    enum TERRAIN terrain;
    bool roads[NUM_DIRECTIONS];
    bool rivers[NUM_DIRECTIONS];
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

    for (int i = 0; i < NUM_DIRECTIONS; i++) {
        tile->roads[i] = false;
        tile->rivers[i] = false;
    }

    return tile;
}

void tile_destroy(struct Tile *tile)
{
    free(tile);
}

unsigned int tile_seed(const struct Tile *tile)
{
    return tile->seed;
}

void tile_set_seed(struct Tile *tile, unsigned int seed)
{
    tile->seed = seed;
}

enum TERRAIN tile_terrain(const struct Tile *tile)
{
    return tile->terrain;
}

void tile_set_terrain(struct Tile *tile, enum TERRAIN t)
{
    if (!tile) {
        return;
    }
    tile->terrain = t;
}

bool tile_road(const struct Tile *tile, enum DIRECTION d)
{
    return tile->roads[d];
}

void tile_set_road(struct Tile *tile, enum DIRECTION d, bool b)
{
    if (!tile) {
        return;
    }
    tile->roads[d] = b;
}

void tile_toggle_road(struct Tile *tile, enum DIRECTION d)
{
    if (!tile) {
        return;
    }
    tile->roads[d] = !tile->roads[d];
}

void tile_clear_roads(struct Tile *tile)
{
    if (!tile) {
        return;
    }

    for (int i = 0; i < NUM_DIRECTIONS; i++) {
        tile_set_road(tile, i, false);
    }
}

bool tile_river(const struct Tile *tile, enum DIRECTION d)
{
    return tile->rivers[d];
}

void tile_set_river(struct Tile *tile, enum DIRECTION d, bool b)
{
    if (!tile) {
        return;
    }
    tile->rivers[d] = b;
}

void tile_toggle_river(struct Tile *tile, enum DIRECTION d)
{
    if (!tile) {
        return;
    }
    tile->rivers[d] = !tile->rivers[d];
}

void tile_clear_rivers(struct Tile *tile)
{
    if (!tile) {
        return;
    }

    for (int i = 0; i < NUM_DIRECTIONS; i++) {
        tile_set_river(tile, i, false);
    }
}

struct Location *tile_location(struct Tile *tile)
{
    if (!tile) {
        return NULL;
    }
    return tile->location;
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
