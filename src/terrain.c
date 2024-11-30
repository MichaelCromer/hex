#include <ncurses.h>
#include <stddef.h>

#include "include/terrain.h"
#include "include/tile.h"
#include "include/atlas.h"

const char *terrain_unknown     = "Unknown";
const char *terrain_water       = "Water";
const char *terrain_mountains   = "Mountains";
const char *terrain_plains      = "Plains";
const char *terrain_hills       = "Hills";
const char *terrain_forest      = "Forest";
const char *terrain_desert      = "Desert";
const char *terrain_jungle      = "Jungle";
const char *terrain_swamp       = "Swamp";
const char *terrain_tundra      = "Tundra";

#define NUM_TERRAIN_CHOPTS 24
const char *terrain_chopts_unknown      = "                     ??*";
const char *terrain_chopts_water        = "         ~~~~~~~~~~~~~~~";
const char *terrain_chopts_mountains    = "                ..^^^AAA";
const char *terrain_chopts_plains       = "            ,,,,,,;;iitt";
const char *terrain_chopts_hills        = "                 tT4nnnn";
const char *terrain_chopts_forest       = "            ttttttTTTT44";
const char *terrain_chopts_desert       = "            ..........nn";
const char *terrain_chopts_jungle       = "       ttttt TTT 444 #%@";
const char *terrain_chopts_swamp        = "                iijj%~%~";
const char *terrain_chopts_tundra       = "                  ..o=-_";

const char *terrain_hints = "q:water w:mountain e:hills a:plains s:forest d:swamp z:desert x:jungle c:tundra";

const char *terrain_name(enum TERRAIN t)
{
    switch (t) {
        case TERRAIN_WATER:
            return terrain_water;
        case TERRAIN_MOUNTAINS:
            return terrain_mountains;
        case TERRAIN_PLAINS:
            return terrain_plains;
        case TERRAIN_HILLS:
            return terrain_hills;
        case TERRAIN_FOREST:
            return terrain_forest;
        case TERRAIN_DESERT:
            return terrain_desert;
        case TERRAIN_JUNGLE:
            return terrain_jungle;
        case TERRAIN_SWAMP:
            return terrain_swamp;
        case TERRAIN_TUNDRA:
            return terrain_tundra;
        default:
            break;
    }
    return terrain_unknown;
}


const char *terrain_chopts(enum TERRAIN t)
{
    switch (t) {
        case TERRAIN_WATER:
            return terrain_chopts_water;
        case TERRAIN_MOUNTAINS:
            return terrain_chopts_mountains;
        case TERRAIN_PLAINS:
            return terrain_chopts_plains;
        case TERRAIN_HILLS:
            return terrain_chopts_hills;
        case TERRAIN_FOREST:
            return terrain_chopts_forest;
        case TERRAIN_DESERT:
            return terrain_chopts_desert;
        case TERRAIN_JUNGLE:
            return terrain_chopts_jungle;
        case TERRAIN_SWAMP:
            return terrain_chopts_swamp;
        case TERRAIN_TUNDRA:
            return terrain_chopts_tundra;
        default:
            return terrain_chopts_unknown;
    }
}


char terrain_getch(enum TERRAIN t, int x, int y, int seed)
{
    const char *chopts = terrain_chopts(t);
    int offset = (int)t;

    return chopts[prng(x, y, seed + offset, NUM_TERRAIN_CHOPTS)];
}


int terrain_colour(enum TERRAIN t)
{
    switch (t) {
        case TERRAIN_WATER:
            return COLOR_BLUE;
        case TERRAIN_MOUNTAINS:
            return COLOR_WHITE;
        case TERRAIN_PLAINS:
            return COLOR_GREEN;
        case TERRAIN_HILLS:
            return COLOR_YELLOW;
        case TERRAIN_FOREST:
            return COLOR_GREEN;
        case TERRAIN_DESERT:
            return COLOR_YELLOW;
        case TERRAIN_JUNGLE:
            return COLOR_GREEN;
        case TERRAIN_SWAMP:
            return COLOR_MAGENTA;
        case TERRAIN_TUNDRA:
            return COLOR_CYAN;
        default:
            return COLOR_WHITE;
    }
}


const char *terrain_statusline(void)
{
    return terrain_hints;
}
