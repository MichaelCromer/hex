#include <ctype.h>
#include <string.h>

#include "include/action.h"
#include "include/interface.h"


void action_move(struct State *s, enum DIRECTION d, int steps)
{
    for (int i = steps; i > 0; i--) {
        map_step(state_map(s), d);
    }

    ui_update_detail(state_ui(s), map_curr(state_map(s)));
    return;
}


void action_paint(struct State *s, enum TERRAIN t)
{
    if (map_curr_terrain(state_map(s)) == TERRAIN_UNKNOWN) { /* TODO this is a BAD check */
        map_create_neighbours(state_map(s), map_curr_coordinate(state_map(s)));
    }
    map_set_terrain(state_map(s), t);
    ui_update_detail(state_ui(s), map_curr(state_map(s)));
}


void action_drag(struct State *s, enum DIRECTION d)
{
    action_move(s, d, 1);
    action_paint(s, map_curr_terrain(state_map(s)));
}

