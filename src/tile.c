#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "include/tile.h"


static unsigned int count = 0;


unsigned int lcg(unsigned int n)
{
    return 16645265*n + 1013904223;
}


unsigned int puid()
{
    return lcg(count++) * lcg(time(NULL));
}


unsigned int prng(
        const unsigned int x, 
        const unsigned int y, 
        const unsigned int seed, 
        const unsigned int max)
{
    return ((((x * 73856093) ^ (y * 19349963) ^ (seed * 83492791)) % max) + max) % max;
}


struct Tile {
    unsigned int seed;
    enum TERRAIN terrain;
    bool roads[NUM_DIRECTIONS];
    bool rivers[NUM_DIRECTIONS];
};


struct Tile *tile_create()
{
    struct Tile *tile = malloc(sizeof(struct Tile));

    tile->seed = puid();
    tile->terrain = TERRAIN_UNKNOWN;

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


int tile_seed(const struct Tile *tile)
{
    return tile->seed;
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


