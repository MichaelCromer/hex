#include <ncurses.h>

#include "hdr/enum.h"

/*******************************************************************************
 *  COLOUR
 ******************************************************************************/

/*  COLOUR : Functions */

void colour_initialise(void)
{
    if (!can_change_color()) return;
    bool full = can_change_color();

    start_color();

    if (!full) {
        init_pair(COLOUR_PAIR_RED, COLOR_RED, COLOR_BLACK);
        init_pair(COLOUR_PAIR_GREEN, COLOR_GREEN, COLOR_BLACK);
        init_pair(COLOUR_PAIR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
        init_pair(COLOUR_PAIR_BLUE, COLOR_BLUE, COLOR_BLACK);
        init_pair(COLOUR_PAIR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
        init_pair(COLOUR_PAIR_CYAN, COLOR_CYAN, COLOR_BLACK);
        init_pair(COLOUR_PAIR_WHITE, COLOR_WHITE, COLOR_BLACK);
        init_pair(COLOUR_PAIR_RIVER, COLOR_BLACK, COLOR_BLUE);
        init_pair(COLOUR_PAIR_ROAD, COLOR_BLACK, COLOR_YELLOW);
        init_pair(COLOUR_PAIR_MOUNTAIN, COLOR_BLACK, COLOR_WHITE);
        init_pair(COLOUR_PAIR_FOREST, COLOR_BLACK, COLOR_GREEN);
    } else {
        init_pair(COLOUR_PAIR_RED, COLOR_RED, COLOR_BLACK);
        init_pair(COLOUR_PAIR_GREEN, COLOR_GREEN, COLOR_BLACK);
        init_pair(COLOUR_PAIR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
        init_pair(COLOUR_PAIR_BLUE, COLOR_BLUE, COLOR_BLACK);
        init_pair(COLOUR_PAIR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
        init_pair(COLOUR_PAIR_CYAN, COLOR_CYAN, COLOR_BLACK);
        init_pair(COLOUR_PAIR_WHITE, COLOR_WHITE, COLOR_BLACK);
        init_pair(COLOUR_PAIR_RIVER, COLOR_BLACK, COLOR_BLUE);
        init_pair(COLOUR_PAIR_ROAD, COLOR_BLACK, COLOR_YELLOW);
        init_pair(COLOUR_PAIR_MOUNTAIN, COLOR_BLACK, COLOR_WHITE);
        init_pair(COLOUR_PAIR_FOREST, COLOR_BLACK, COLOR_GREEN);
    }
}

/*******************************************************************************
 *  DIRECTION
 ******************************************************************************/

/*  DIRECTION : Functions */

enum DIRECTION direction_opposite(enum DIRECTION d)
{
    return (d + (NUM_DIRECTIONS/2)) % NUM_DIRECTIONS;
}

enum DIRECTION direction_next(enum DIRECTION d)
{
    return (d + 1) % NUM_DIRECTIONS;
}

enum DIRECTION direction_prev(enum DIRECTION d)
{
    return (d + NUM_DIRECTIONS - 1) % NUM_DIRECTIONS;
}

/*******************************************************************************
 *  TERRAIN
 ******************************************************************************/

/*  TERRAIN : Constants */

const char *terrain_unknown = "Unknown";
const char *terrain_water = "Water";
const char *terrain_mountains = "Mountains";
const char *terrain_plains = "Plains";
const char *terrain_hills = "Hills";
const char *terrain_forest = "Forest";
const char *terrain_desert = "Desert";
const char *terrain_jungle = "Jungle";
const char *terrain_swamp = "Swamp";
const char *terrain_tundra = "Tundra";

const char *terrain_chopts_unknown = "                     ??*";
const char *terrain_chopts_water = "         ~~~~~~~~~~~~~~~";
const char *terrain_chopts_mountains = "                ..^^^AAA";
const char *terrain_chopts_plains = "            ,,,,,,;;iitt";
const char *terrain_chopts_hills = "                 tT4nnnn";
const char *terrain_chopts_forest = "            ttttttTTTT44";
const char *terrain_chopts_desert = "            ..........nn";
const char *terrain_chopts_jungle = "       ttttt TTT 444 #%@";
const char *terrain_chopts_swamp = "                iijj%~%~";
const char *terrain_chopts_tundra = "                  ..o=-_";

/*  TERRAIN : Functions */

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

int terrain_colour(enum TERRAIN t, char c)
{
    if (c == ' ') return COLOR_WHITE;

    switch (t) {
        case TERRAIN_WATER:
            return COLOR_BLUE;
        case TERRAIN_MOUNTAINS:
            return COLOUR_PAIR_MOUNTAIN;
        case TERRAIN_PLAINS:
            return COLOR_GREEN;
        case TERRAIN_HILLS:
            return COLOR_YELLOW;
        case TERRAIN_FOREST:
            return COLOUR_PAIR_FOREST;
        case TERRAIN_DESERT:
            return COLOR_YELLOW;
        case TERRAIN_JUNGLE:
            return COLOR_GREEN;
        case TERRAIN_SWAMP:
            return COLOR_MAGENTA;
        case TERRAIN_TUNDRA:
            return COLOR_CYAN;
        default:
            break;
    }
    return COLOR_WHITE;
}

bool terrain_impassable(enum TERRAIN t)
{
    switch (t) {
        case TERRAIN_UNKNOWN:
        case TERRAIN_WATER:
            return true;
        default:
            break;
    }
    return false;
}

/*******************************************************************************
 *  MODE
 ******************************************************************************/

/*  MODE : Constants */

const char *modestr_navigate = "[NAVIGATE]";
const char *modestr_terrain = "[TERRAIN]";
const char *modestr_command = "[COMMAND]";
const char *modestr_road = "[ROADS]";
const char *modestr_river = "[RIVERS]";
const char *modestr_location = "[LOCATION]";
const char *modestr_await_terrain = "(terrain)";
const char *modestr_await_road = "(roads)";
const char *modestr_await_river = "(rivers)";
const char *modestr_await_location = "(location)";
const char *modestr_unknown = "???";

/*  MODE : Functions */

const char *mode_name(enum MODE m)
{
    switch (m) {
        case MODE_NAVIGATE:
            return modestr_navigate;
        case MODE_TERRAIN:
            return modestr_terrain;
        case MODE_AWAIT_TERRAIN:
            return modestr_await_terrain;
        case MODE_AWAIT_ROAD:
            return modestr_await_road;
        case MODE_ROAD:
            return modestr_road;
        case MODE_AWAIT_RIVER:
            return modestr_await_river;
        case MODE_RIVER:
            return modestr_river;
        case MODE_AWAIT_LOCATION:
            return modestr_await_location;
        case MODE_LOCATION:
            return modestr_location;
        case MODE_COMMAND:
            return modestr_command;
        default:
            break;
    }
    return modestr_unknown;
}

int mode_colour(enum MODE m)
{
    switch (m) {
        case MODE_TERRAIN:
        case MODE_AWAIT_TERRAIN:
            return COLOR_GREEN;
        case MODE_ROAD:
        case MODE_AWAIT_ROAD:
            return COLOR_YELLOW;
        case MODE_RIVER:
        case MODE_AWAIT_RIVER:
            return COLOR_CYAN;
        case MODE_LOCATION:
        case MODE_AWAIT_LOCATION:
            return COLOR_MAGENTA;
        case MODE_COMMAND:
            return COLOR_RED;
        case MODE_NAVIGATE:
            return COLOR_WHITE;
        default:
            break;
    }
    return COLOR_WHITE;
}

bool mode_is_await(enum MODE m)
{
    switch (m) {
        case MODE_AWAIT_LOCATION:
        case MODE_AWAIT_RIVER:
        case MODE_AWAIT_ROAD:
        case MODE_AWAIT_TERRAIN:
            return true;
        default:
            break;
    }
    return false;;
}


enum UI_PANEL mode_panel(enum MODE m)
{
    switch (m) {
        case MODE_NAVIGATE:
            return PANEL_NAVIGATE;
        case MODE_TERRAIN:
            return PANEL_TERRAIN;
        case MODE_AWAIT_TERRAIN:
            return PANEL_AWAIT_TERRAIN;
        case MODE_ROAD:
            return PANEL_ROAD;
        case MODE_AWAIT_ROAD:
            return PANEL_AWAIT_ROAD;
        case MODE_RIVER:
            return PANEL_RIVER;
        case MODE_AWAIT_RIVER:
            return PANEL_AWAIT_RIVER;
        case MODE_LOCATION:
            return PANEL_LOCATION;
        case MODE_AWAIT_LOCATION:
            return PANEL_AWAIT_LOCATION;
        case MODE_COMMAND:
            return PANEL_COMMAND;
        default: return PANEL_NONE;
    }
}
