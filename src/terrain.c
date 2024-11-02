#include <ncurses.h>
#include <stddef.h>

#include "include/terrain.h"
#include "include/grid.h"

const char *terrain_unknown = "Unknown";
const char *terrain_water = "Water";
const char *terrain_mountains = "Mountains";
const char *terrain_plains = "Plains";
const char *terrain_hills = "Hills";
const char *terrain_forest = "Forest";
const char *terrain_desert = "Desert";
const char *terrain_jungle = "Jungle";
const char *terrain_swamp = "Swamp";

#define NUM_TERRAIN_CHOPTS 12
const char *terrain_chopts_unknown      = "          ?*";
const char *terrain_chopts_water        = "    ~~~~~~~~";
const char *terrain_chopts_mountains    = "    ..^^^AAA";
const char *terrain_chopts_plains       = "      ,,,;;t";
const char *terrain_chopts_hills        = "      ,;nnnn";
const char *terrain_chopts_forest       = "    ttttTT44";
const char *terrain_chopts_desert       = "    ......nn";
const char *terrain_chopts_jungle       = "    tT4&%$#@";
const char *terrain_chopts_swamp        = "    iijj%~%~";


const char *terrain_name(enum TERRAIN t)
{
    switch (t) {
        case WATER:
            return terrain_water;
        case MOUNTAINS:
            return terrain_mountains;
        case PLAINS:
            return terrain_plains;
        case HILLS:
            return terrain_hills;
        case FOREST:
            return terrain_forest;
        case DESERT:
            return terrain_desert;
        case JUNGLE:
            return terrain_jungle;
        case SWAMP:
            return terrain_swamp;
        default:
            break;
    }
    return terrain_unknown;
}


const char *terrain_chopts(enum TERRAIN t)
{
    switch (t) {
        case WATER:
            return terrain_chopts_water;
        case MOUNTAINS:
            return terrain_chopts_mountains;
        case PLAINS:
            return terrain_chopts_plains;
        case HILLS:
            return terrain_chopts_hills;
        case FOREST:
            return terrain_chopts_forest;
        case DESERT:
            return terrain_chopts_desert;
        case JUNGLE:
            return terrain_chopts_jungle;
        case SWAMP:
            return terrain_chopts_swamp;
        default:
            return terrain_chopts_unknown;
    }
}


char terrain_getch(enum TERRAIN t, int x, int y, int seed)
{
    const char *chopts = terrain_chopts(t);
    int offset = (int)t;

    return chopts[seed_prng(x, y, seed + offset, NUM_TERRAIN_CHOPTS)];
}


int terrain_colour(enum TERRAIN t)
{
    switch (t) {
        case WATER:
            return COLOR_BLUE;
        case MOUNTAINS:
            return COLOR_WHITE;
        case PLAINS:
            return COLOR_GREEN;
        case HILLS:
            return COLOR_YELLOW;
        case FOREST:
            return COLOR_GREEN;
        case DESERT:
            return COLOR_YELLOW;
        case JUNGLE:
            return COLOR_GREEN;
        case SWAMP:
            return COLOR_CYAN;
        default:
            return COLOR_WHITE;
    }
}
