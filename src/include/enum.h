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
    INPUT_MODE_ROAD,
    INPUT_MODE_COMMAND
};
const char *mode_name(enum INPUT_MODE m);
int mode_colour(enum INPUT_MODE m);


#define NUM_TERRAIN 9
#define NUM_TERRAIN_CHOPTS 24
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
const char *terrain_name(enum TERRAIN t);
const char *terrain_chopts(enum TERRAIN t);
int terrain_colour(enum TERRAIN t);
const char *terrain_statusline(void);


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


#define NUM_CHILDREN 9
enum CHILDREN {
    CHILD0,
    CHILD1,
    CHILD2,
    CHILD3,
    CHILD4,
    CHILD5,
    CHILD6,
    CHILD7,
    CHILD8
};


enum COMMAND_TYPE {
    COMMAND_TYPE_NONE,
    COMMAND_TYPE_ERROR,
    COMMAND_TYPE_QUIT,
    COMMAND_TYPE_WRITE,
    COMMAND_TYPE_EDIT,
};


enum DIRECTION direction_opposite(enum DIRECTION d);

#endif 
