#include <ctype.h>

#include "include/key.h"

enum DIRECTION key_direction(key k)
{
    switch (tolower(k)) {
        case KEY_EE:
            return DIRECTION_EE;
        case KEY_NE:
            return DIRECTION_NE;
        case KEY_NW:
            return DIRECTION_NW;
        case KEY_WW:
            return DIRECTION_WW;
        case KEY_SW:
            return DIRECTION_SW;
        case KEY_SE:
            return DIRECTION_SE;
        default:
            break;
    }
    return DIRECTION_XX;
}

bool key_is_direction(key k)
{
    switch (tolower(k)) {
        case KEY_EE:
        case KEY_NE:
        case KEY_NW:
        case KEY_WW:
        case KEY_SW:
        case KEY_SE:
            return true;
        default:
            break;
    }
    return false;
}

bool key_is_special(key k)
{
    switch (k) {
        case KEY_EE_SPECIAL:
        case KEY_NE_SPECIAL:
        case KEY_NW_SPECIAL:
        case KEY_WW_SPECIAL:
        case KEY_SW_SPECIAL:
        case KEY_SE_SPECIAL:
            return true;
        default:
            break;
    }
    return false;
}

enum TERRAIN key_terrain(key k)
{
    switch (k) {
        case KEY_TERRAIN_UNKNOWN:
            return TERRAIN_UNKNOWN;
        case KEY_TERRAIN_WATER:
            return TERRAIN_WATER;
        case KEY_TERRAIN_MOUNTAINS:
            return TERRAIN_MOUNTAINS;
        case KEY_TERRAIN_PLAINS:
            return TERRAIN_PLAINS;
        case KEY_TERRAIN_HILLS:
            return TERRAIN_HILLS;
        case KEY_TERRAIN_FOREST:
            return TERRAIN_FOREST;
        case KEY_TERRAIN_DESERT:
            return TERRAIN_DESERT;
        case KEY_TERRAIN_JUNGLE:
            return TERRAIN_JUNGLE;
        case KEY_TERRAIN_SWAMP:
            return TERRAIN_SWAMP;
        case KEY_TERRAIN_TUNDRA:
            return TERRAIN_TUNDRA;
        default:
            break;
    }
    return TERRAIN_NONE;
}

bool key_is_terrain(key k)
{
    switch (k) {
        case KEY_TERRAIN_UNKNOWN:
        case KEY_TERRAIN_WATER:
        case KEY_TERRAIN_MOUNTAINS:
        case KEY_TERRAIN_PLAINS:
        case KEY_TERRAIN_HILLS:
        case KEY_TERRAIN_FOREST:
        case KEY_TERRAIN_DESERT:
        case KEY_TERRAIN_JUNGLE:
        case KEY_TERRAIN_SWAMP:
        case KEY_TERRAIN_TUNDRA:
            return true;
        default:
            break;
    }
    return false;
}

bool key_is_mode(key k)
{
    switch (k) {
        case KEY_ESCAPE:
        case KEY_MODE_COMMAND:
        case KEY_MODE_AWAIT_TERRAIN:
        case KEY_MODE_TERRAIN:
        case KEY_MODE_AWAIT_ROAD:
        case KEY_MODE_ROAD:
        case KEY_MODE_AWAIT_RIVER:
        case KEY_MODE_RIVER:
            return true;
        default:
            break;
    }
    return false;
}

enum MODE key_mode(key k)
{
    switch (k) {
        case KEY_MODE_COMMAND:
            return MODE_COMMAND;
        case KEY_MODE_AWAIT_TERRAIN:
            return MODE_AWAIT_TERRAIN;
        case KEY_MODE_TERRAIN:
            return MODE_TERRAIN;
        case KEY_MODE_AWAIT_ROAD:
            return MODE_AWAIT_ROAD;
        case KEY_MODE_ROAD:
            return MODE_ROAD;
        case KEY_MODE_AWAIT_RIVER:
            return MODE_AWAIT_RIVER;
        case KEY_MODE_RIVER:
            return MODE_RIVER;
        default:
            break;
    }
    return MODE_NONE;
}
