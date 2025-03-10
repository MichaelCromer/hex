#ifndef ENUM_H
#define ENUM_H

#include <stdbool.h>

enum UI_COLOUR {
    COLOUR_NONE,
    COLOUR_SOME,
    COLOUR_MANY
};
enum UI_COLOUR colour_test(void);

#define NUM_UI_PANELS 13
enum UI_PANEL {
    PANEL_SPLASH,
    PANEL_DETAIL,
    PANEL_HINT,
    PANEL_NAVIGATE,
    PANEL_TERRAIN,
    PANEL_AWAIT_TERRAIN,
    PANEL_ROAD,
    PANEL_AWAIT_ROAD,
    PANEL_RIVER,
    PANEL_AWAIT_RIVER,
    PANEL_LOCATION,
    PANEL_AWAIT_LOCATION,
    PANEL_COMMAND,
    PANEL_NONE,
};


enum MODE {
    MODE_CAPTURE,
    MODE_NAVIGATE,
    MODE_TERRAIN,
    MODE_AWAIT_TERRAIN,
    MODE_ROAD,
    MODE_AWAIT_ROAD,
    MODE_RIVER,
    MODE_AWAIT_RIVER,
    MODE_LOCATION,
    MODE_AWAIT_LOCATION,
    MODE_COMMAND,
    MODE_NONE,
};
const char *mode_name(enum MODE m);
int mode_colour(enum MODE m);
bool mode_is_await(enum MODE m);
enum UI_PANEL mode_panel(enum MODE m);


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
bool terrain_impassable(enum TERRAIN t);
const char *terrain_statusline(void);

enum LOCATION {
    LOCATION_NONE,
    LOCATION_SETTLEMENT,
    LOCATION_FEATURE,
    LOCATION_DUNGEON
};

#define NUM_DIRECTIONS 6
enum DIRECTION {
    DIRECTION_EE,
    DIRECTION_NE,
    DIRECTION_NW,
    DIRECTION_WW,
    DIRECTION_SW,
    DIRECTION_SE,
    DIRECTION_XX
};
enum DIRECTION direction_opposite(enum DIRECTION d);
enum DIRECTION direction_next(enum DIRECTION d);
enum DIRECTION direction_prev(enum DIRECTION d);

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

enum COMMAND {
    COMMAND_NONE,
    COMMAND_ERROR,
    COMMAND_QUIT,
    COMMAND_WRITE,
    COMMAND_EDIT,
};

#endif
