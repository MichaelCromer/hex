#include <ctype.h>

#include "include/key.h"


enum DIRECTION key_direction(char ch)
{
    switch (tolower(ch)) {
        case KEY_EE:
            return EAST;
        case KEY_NE:
            return NORTHEAST;
        case KEY_NW:
            return NORTHWEST;
        case KEY_WW:
            return WEST;
        case KEY_SW:
            return SOUTHWEST;
        case KEY_SE:
            return SOUTHEAST;
        default:
            return DIRECTION_NONE;
    }
}


bool key_is_direction(char ch)
{
    switch (tolower(ch)) {
        case KEY_EE:
        case KEY_NE:
        case KEY_NW:
        case KEY_WW:
        case KEY_SW:
        case KEY_SE:
            return true;
        default:
            return false;
    }
}


bool key_is_special(char ch)
{
    switch (ch) {
        case KEY_EE_SPECIAL:
        case KEY_NE_SPECIAL:
        case KEY_NW_SPECIAL:
        case KEY_WW_SPECIAL:
        case KEY_SW_SPECIAL:
        case KEY_SE_SPECIAL:
            return true;
        default:
            return false;
    }
}


enum TERRAIN key_terrain(char ch)
{
    switch (ch) {
        case KEY_TERRAIN_UNKNOWN:
            return TERRAIN_UNKNOWN;
        case KEY_TERRAIN_WATER:
            return WATER;
        case KEY_TERRAIN_MOUNTAINS:
            return MOUNTAINS;
        case KEY_TERRAIN_PLAINS:
            return PLAINS;
        case KEY_TERRAIN_HILLS:
            return HILLS;
        case KEY_TERRAIN_FOREST:
            return FOREST;
        case KEY_TERRAIN_DESERT:
            return DESERT;
        case KEY_TERRAIN_JUNGLE:
            return JUNGLE;
        case KEY_TERRAIN_SWAMP:
            return SWAMP;
        default:
            return TERRAIN_NONE;
    }
}


enum TERRAIN key_is_terrain(char ch)
{
    switch (ch) {
        case KEY_TERRAIN_UNKNOWN:
        case KEY_TERRAIN_WATER:
        case KEY_TERRAIN_MOUNTAINS:
        case KEY_TERRAIN_PLAINS:
        case KEY_TERRAIN_HILLS:
        case KEY_TERRAIN_FOREST:
        case KEY_TERRAIN_DESERT:
        case KEY_TERRAIN_JUNGLE:
        case KEY_TERRAIN_SWAMP:
            return true;
        default:
            return false;
    }
}


