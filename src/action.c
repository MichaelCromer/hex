#include <ctype.h>
#include <string.h>

#include "include/action.h"
#include "include/commandline.h"
#include "include/interface.h"
#include "include/tile.h"
#include "include/file.h"


void action_write(struct State *state, const char *filename)
{
    FILE *file = fopen(filename, "w");
    if (!file) {
        return;
    }

    write_state(file, state);
    fclose(file);
}

void action_edit(struct State *state, const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file) {
        return;
    }

    read_state(file, state);
    fclose(file);
}

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

    ui_update_detail(atlas);
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
            neighbour = chart_tile(atlas_neighbour(atlas, i));
            if (tile_road(tile, i)) {
                tile_set_road(neighbour, direction_opposite(i), false);
            }
            if (tile_river(tile, i)) {
                tile_set_river(neighbour, direction_opposite(i), false);
            }
        }

        tile_clear_roads(tile);
        tile_clear_rivers(tile);

        if (tile_location(tile)) {
            location_set_type(tile_location(tile), LOCATION_NONE);
        }
    }

    ui_update_detail(atlas);
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

void action_paint_location(struct State *state, enum LOCATION t)
{
    struct Tile *tile = atlas_tile(state_atlas(state));

    if (terrain_impassable(tile_terrain(tile))) {
        return;
    }

    if (!tile_location(tile)) {
        atlas_create_location(state_atlas(state), t);
    } else if (location_type(tile_location(tile)) == t) {
        location_set_type(tile_location(tile), LOCATION_NONE);
    } else {
        location_set_type(tile_location(tile), t);
    }
}

void action_capture(struct State *state, key k)
{
    if (k != KEY_ENTER && k != '\n') return;

    if (ui_is_show(PANEL_SPLASH)) ui_toggle_show(PANEL_SPLASH);

    state_push_mode(state, MODE_NAVIGATE);
}

void action_navigate(struct State *state, key k)
{
    if (key_is_direction(k)) {
        action_move(state, key_direction(k), (key_is_special(k)) ? 3 : 1);
        return;
    }

    if (key_is_mode(k)) action_mode(state, key_mode(k));

    switch (k) {
        case KEY_TOGGLE_DETAIL:
            ui_toggle_show(PANEL_DETAIL);
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
        enum TERRAIN t = atlas_terrain(state_atlas(state));
        action_move(state, key_direction(k), 1);

        if (key_is_special(k)) {
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
    switch (k) {
        case KEY_ESCAPE:
            commandline_reset();
            state_pop_mode(state);
            return;

        case KEY_ENTER:
        case '\n':
            commandline_parse();
            switch (commandline_type()) {
                case COMMAND_QUIT:
                    state_set_quit(state, true);
                    break;
                case COMMAND_WRITE:
                    action_write(state, commandline_data());
                    break;
                case COMMAND_EDIT:
                    action_edit(state, commandline_data());
                default:
                    break;
            }

            commandline_reset();
            state_pop_mode(state);
            return;

        case KEY_BACKSPACE:
        case '\b':
        case 127:
            if (commandline_len() <= 0) {
                commandline_reset();
                state_pop_mode(state);
            }

            commandline_popch();
            return;

        default:
            commandline_putch(state_currkey(state));
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

void action_location(struct State *state, key k)
{
    if (state_await(state)) {
        action_mode(state, key_mode(k));
    }

    if (key_is_location(k)) {
        action_paint_location(state, key_location(k));
    }

    if (key_is_direction(k)) {
        action_move(state, key_direction(k), 1);
    }

    if (key_is_mode(k)) {
        action_mode(state, key_mode(k));
        return;
    }
}
