#ifndef ENUM_H
#define ENUM_H


enum UI_COLOUR {
    COLOUR_NONE,
    COLOUR_SOME,
    COLOUR_MANY
};


#define NUM_UI_PANELS 3
enum UI_PANEL {
   PANEL_SPLASH,
   PANEL_TERRAIN,
   PANEL_DETAIL
};


enum INPUT_MODE {
    INPUT_MODE_NONE,
    INPUT_MODE_CAPTURE,
    INPUT_MODE_NAVIGATE,
    INPUT_MODE_TERRAIN,
    INPUT_MODE_COMMAND
};


#define NUM_TERRAIN 9
enum TERRAIN {
    TERRAIN_NONE,
    TERRAIN_UNKNOWN,
    TERRAIN_WATER,
    TERRAIN_MOUNTAINS,
    TERRAIN_PLAINS,
    TERRAIN_HILLS,
    TERRAIN_FOREST,
    TERRAIN_DESERT,
    TERRAIN_JUNGLE,
    TERRAIN_SWAMP,
    TERRAIN_TUNDRA
};


#define NUM_DIRECTIONS 6
enum DIRECTION {
    EAST,
    NORTHEAST,
    NORTHWEST,
    WEST,
    SOUTHWEST,
    SOUTHEAST,
    DIRECTION_NONE
};


enum COMMAND_TYPE {
    COMMAND_TYPE_NONE,
    COMMAND_TYPE_ERROR,
    COMMAND_TYPE_QUIT,
    COMMAND_TYPE_WRITE,
    COMMAND_TYPE_EDIT,
};


#endif 
