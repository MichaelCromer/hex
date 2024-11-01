#ifndef ENUM_H
#define ENUM_H


enum UI_COLOUR {
    COLOUR_NONE,
    COLOUR_SOME,
    COLOUR_MANY
};


#define UI_NUM_PANELS 3
enum UI_PANEL {
   PANEL_SPLASH,
   PANEL_TERRAIN,
   PANEL_DETAIL
};


enum INPUTMODE {
    INPUT_NONE,
    INPUT_CAPTURE,
    INPUT_NAVIGATE,
    INPUT_TERRAIN
};


#define NUM_TERRAIN 9
enum TERRAIN {
    TERRAIN_NONE,
    TERRAIN_UNKNOWN,
    WATER,
    MOUNTAINS,
    PLAINS,
    HILLS,
    FOREST,
    DESERT,
    JUNGLE,
    SWAMP
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


#endif 
