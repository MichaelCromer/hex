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

    if ((m == state_mode(state)) || (m == MODE_NONE)) {
        state_push_mode(state, MODE_NAVIGATE);
    } else {
        state_push_mode(state, m);
    }
}


void action_move(struct State *state, enum DIRECTION d, int steps)
{
    struct Atlas *atlas = state_atlas(state);

    for (int i = steps; i > 0; i--) {
        atlas_step(atlas, d);
    }

    ui_update_detail(state_ui(state), atlas_curr(atlas));
    return;
}


void action_paint_terrain(struct State *state, enum TERRAIN t)
{
    struct Atlas *atlas = state_atlas(state);

    if (atlas_terrain(atlas) == TERRAIN_UNKNOWN) {
        atlas_create_neighbours(atlas);
    }

    atlas_set_terrain(atlas, t);

    if (terrain_impassable(t)) {
        struct Tile *tile = atlas_tile(atlas);
        struct Tile *neighbour = NULL;

        for (int i = 0; i < NUM_DIRECTIONS; i++) {
            if (!tile_road(tile, i)) {
                continue;
            }
            neighbour = chart_tile(atlas_neighbour(atlas, i));
            tile_set_road(neighbour, direction_opposite(i), false);
        }

        tile_clear_roads(atlas_tile(atlas));
    }

    ui_update_detail(state_ui(state), atlas_curr(atlas));
}


void action_paint_road(struct State *state, enum DIRECTION d)
{
    struct Atlas *atlas = state_atlas(state);
    struct Tile *tile = atlas_tile(atlas);
    struct Tile *neighbour = chart_tile(atlas_neighbour(atlas, d));

    action_move(state, d, 1);

    if (terrain_impassable(tile_terrain(tile))
            || terrain_impassable(tile_terrain(neighbour))) {
        return;
    }

    tile_toggle_road(tile, d);
    tile_toggle_road(neighbour, direction_opposite(d));
}


void action_paint_river(struct State *state, enum DIRECTION d)
{
    struct Atlas *atlas = state_atlas(state);
    struct Tile *tile = atlas_tile(atlas);
    struct Tile *neighbour = chart_tile(atlas_neighbour(atlas, d));

    if (terrain_impassable(tile_terrain(tile))
            || terrain_impassable(tile_terrain(neighbour))) {
        return;
    }

    tile_toggle_river(tile, d);
    tile_toggle_river(neighbour, direction_opposite(d));
}


void action_drag_river(struct State *state, enum DIRECTION d)
{
    struct Tile *tile = atlas_tile(state_atlas(state));

    if (tile_river(tile, direction_next(d)) || tile_river(tile, direction_prev(d))) {
        action_move(state, d, 1);

        if (tile_river(tile, direction_next(d))) {
            enum DIRECTION e = direction_prev(direction_opposite(d));
            action_paint_river(state, e);
        }

        if (tile_river(tile, direction_prev(d))) {
            enum DIRECTION e = direction_next(direction_opposite(d));
            action_paint_river(state, e);
        }

        return;
    }

    enum DIRECTION e = direction_opposite(d);
    if (tile_river(tile, direction_next(e)) || tile_river(tile, direction_prev(e))) {

        if (tile_river(tile, direction_next(e))) {
            enum DIRECTION f = direction_next(direction_next(e));
            action_paint_river(state, f);
        }

        if (tile_river(tile, direction_prev(e))) {
            enum DIRECTION f = direction_prev(direction_prev(e));
            action_paint_river(state, f);
        }

        return;
    }

    action_move(state, d, 1);
}


void action_capture(struct State *state, key k)
{
    if (k != KEY_ENTER && k != '\n') {
        return;
    }

    if (ui_show(state_ui(state), PANEL_SPLASH)) {
        ui_toggle(state_ui(state), PANEL_SPLASH);
    }

    state_push_mode(state, MODE_NAVIGATE);
}


void action_navigate(struct State *state, key k)
{
    if (key_is_direction(k)) {
        action_move(state, key_direction(k), (key_is_special(k)) ? 3 : 1);
        return;
    }

    if (key_is_mode(k)) {
        action_mode(state, key_mode(k));
    }

    switch (k) {
        case KEY_TOGGLE_DETAIL:
            ui_toggle(state_ui(state), PANEL_DETAIL);
            return;
        default:
            break;
    }
}


void action_terrain(struct State *state, key k)
{
    if (state_await(state)) {
        if (key_is_terrain(k)) {
            action_paint_terrain(state, key_terrain(k));
        }

        action_mode(state, key_mode(k));
        return;
    }

    if (key_is_terrain(k)) {
        action_paint_terrain(state, key_terrain(k));
        return;
    }

    if (key_is_direction(k)) {
        action_move(state, key_direction(k), 1);

        if (key_is_special(k)) {
            enum TERRAIN t = atlas_terrain(state_atlas(state));

            if (t != TERRAIN_UNKNOWN) {
                action_paint_terrain(state, t);
            }
        }

        return;
    }

    if (key_is_mode(k)) {
        action_mode(state, key_mode(k));
    }
}


void action_command(struct State *state, key k)
{
    struct Commandline *commandline = state_commandline(state);
    struct Command *c = NULL;

    switch (k) {
        case KEY_ESCAPE:
            commandline_reset(commandline);
            state_pop_mode(state);
            return;

        case KEY_ENTER:
        case '\n':
            c = commandline_parse(commandline);
            switch (command_type(c)) {
                case COMMAND_QUIT:
                    state_set_quit(state, true);
                default:
                    break;
            }

            commandline_reset(commandline);
            state_pop_mode(state);
            command_destroy(c);
            return;

        case KEY_BACKSPACE:
        case '\b':
        case 127:
            if (commandline_len(commandline) <= 0) {
                commandline_reset(commandline);
                state_pop_mode(state);
            }

            commandline_popch(commandline);
            return;

        default:
            commandline_putch(commandline, state_currkey(state));
            break;
    }
}


void action_road(struct State *state, key k)
{
    if (state_await(state)) {
        if (key_is_direction(k)) {
            action_paint_road(state, key_direction(k));
        }

        action_mode(state, key_mode(k));
        return;
    }

    if (key_is_direction(k)) {
        if (key_is_special(k)) {
            action_paint_road(state, key_direction(k));
        } else {
            action_move(state, key_direction(k), 1);
        }
        return;
    }

    if (key_is_mode(k)) {
        action_mode(state, key_mode(k));
    }
}


void action_river(struct State *state, key k)
{
    if (state_await(state)) {
        if (key_is_direction(k)) {
            action_paint_river(state, key_direction(k));
        }

        action_mode(state, key_mode(k));
        return;
    }

    if (key_is_direction(k)) {
        if (key_is_special(k)) {
            action_drag_river(state, key_direction(k));
        } else {
            action_move(state, key_direction(k), 1);
        }
        return;
    }

    if (key_is_mode(k)) {
        action_mode(state, key_mode(k));
        return;
    }
}
