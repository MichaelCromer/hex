#include <ctype.h>
#include <string.h>

#include "include/action.h"
#include "include/commandline.h"
#include "include/interface.h"
#include "include/tile.h"


void action_mode(struct State *state, enum MODE m)
{
    if (state_await(state)) {
        state_pop_mode(state);
        return;
    }

    /* don't set an await mode for the currently select mode! */
    if (state_mode(state) == mode_drop_await(m)) {
        return;
    }

    if ((m == state_mode(state)) || (m == MODE_NONE)) {
        state_push_mode(state, MODE_NAVIGATE);
    } else {
        state_push_mode(state, m);
    }
}


void action_move(struct State *s, enum DIRECTION d, int steps)
{
    for (int i = steps; i > 0; i--) {
        atlas_step(state_atlas(s), d);
    }

    ui_update_detail(state_ui(s), atlas_curr(state_atlas(s)));
    return;
}


void action_paint_terrain(struct State *s, enum TERRAIN t)
{
    if (atlas_terrain(state_atlas(s)) == TERRAIN_UNKNOWN) { /* TODO this is a BAD check */
        atlas_create_neighbours(state_atlas(s));
    }
    atlas_set_terrain(state_atlas(s), t);
    if (terrain_impassable(t)) {
        tile_clear_roads(atlas_tile(state_atlas(s)));
    }
    ui_update_detail(state_ui(s), atlas_curr(state_atlas(s)));
}


void action_paint_road(struct State *s, enum DIRECTION d)
{
    struct Tile *tile = atlas_tile(state_atlas(s));
    action_move(s, d, 1);
    if (!terrain_impassable(tile_terrain(tile))) {
        tile_toggle_road(tile, d);
    }
    if (!terrain_impassable(atlas_terrain(state_atlas(s)))) {
        tile_toggle_road(atlas_tile(state_atlas(s)), direction_opposite(d));
    }
}


void action_capture(struct State *s, key k)
{
    if (k != KEY_ENTER && k != '\n') {
        return;
    }

    if (ui_show(state_ui(s), PANEL_SPLASH)) {
        ui_toggle(state_ui(s), PANEL_SPLASH);
    }

    state_push_mode(s, MODE_NAVIGATE);
}


void action_navigate(struct State *s, key k)
{
    if (key_is_direction(k)) {
        action_move(s, key_direction(k), (key_is_special(k)) ? 3 : 1);
        return;
    }

    if (key_is_mode(k)) {
        action_mode(s, key_mode(k));
    }

    switch (k) {
        case KEY_TOGGLE_DETAIL:
            ui_toggle(state_ui(s), PANEL_DETAIL);
            return;
        default:
            break;
    }

    return;
}


void action_terrain(struct State *s, key k)
{
    if (state_await(s)) {
        if (key_is_terrain(k)) {
            action_paint_terrain(s, key_terrain(k));
        }
        action_mode(s, key_mode(k));
        return;
    }

    if (key_is_terrain(k)) {
        action_paint_terrain(s, key_terrain(k));
        return;
    }

    if (key_is_direction(k)) {
        enum DIRECTION d = key_direction(k);
        enum TERRAIN t = atlas_terrain(state_atlas(s));
        action_move(s, d, 1);

        if (key_is_special(k)) {
            if (t != TERRAIN_UNKNOWN) {
                action_paint_terrain(s, t);
            }
        }
    }

    if (key_is_mode(k)) {
        action_mode(s, key_mode(k));
    }
    return;
}


void action_command(struct State *s, key k)
{
    struct Command *c = NULL;
    switch (k) {
        case KEY_ESCAPE:
            commandline_reset(state_commandline(s));
            state_pop_mode(s);
            return;

        /* send command to be parsed */
        case KEY_ENTER:
        case '\n':
            c = commandline_parse(state_commandline(s));
            switch (command_type(c)) {
                case COMMAND_QUIT:
                    state_set_quit(s, true);
                default:
                    break;
            }
            commandline_reset(state_commandline(s));
            state_pop_mode(s);
            command_destroy(c);
            return;

        /* delete character from command line */
        case KEY_BACKSPACE:
        case '\b':
        case 127:
            if (commandline_len(state_commandline(s)) <= 0) {
                commandline_reset(state_commandline(s));
                state_pop_mode(s);
            }
            commandline_popch(state_commandline(s));
            return;

        /* add char to buffer */
        default:
            commandline_putch(state_commandline(s), state_currkey(s));
            break;
    }

    return;
}


void action_road(struct State *s, key k)
{
    if (state_await(s)) {
        if (key_is_direction(k)) {
            action_paint_road(s, key_direction(k));
        }
        action_mode(s, key_mode(k));
        return;
    }

    if (key_is_direction(k)) {
        if (key_is_special(k)) {
            action_paint_road(s, key_direction(k));
        } else {
            action_move(s, key_direction(k), 1);
        }
    }

    if (key_is_mode(k)) {
        action_mode(s, key_mode(k));
    }
    return;
}
