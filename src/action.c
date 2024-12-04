#include <ctype.h>
#include <string.h>

#include "include/action.h"
#include "include/commandline.h"
#include "include/interface.h"
#include "include/tile.h"


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
    struct Atlas *atlas = state_atlas(s);

    if (atlas_terrain(atlas) == TERRAIN_UNKNOWN) { /* TODO this is a BAD check */
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
    ui_update_detail(state_ui(s), atlas_curr(atlas));
}


void action_paint_road(struct State *s, enum DIRECTION d)
{
    struct Atlas *atlas = state_atlas(s);
    struct Tile *tile = atlas_tile(atlas);
    struct Tile *neighbour = chart_tile(atlas_neighbour(atlas, d));
    action_move(s, d, 1);
    if (terrain_impassable(tile_terrain(tile))
            || terrain_impassable(tile_terrain(neighbour))) {
        return;
    }
    tile_toggle_road(tile, d);
    tile_toggle_road(neighbour, direction_opposite(d));
}


void action_paint_river(struct State *s, enum DIRECTION d)
{
    struct Atlas *atlas = state_atlas(s);
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


void action_capture(struct State *s, key k)
{
    if (k != KEY_ENTER && k != '\n') {
        return;
    }

    if (ui_show(state_ui(s), PANEL_SPLASH)) {
        ui_toggle(state_ui(s), PANEL_SPLASH);
    }

    state_set_mode(s, MODE_NAVIGATE);
}


void action_navigate(struct State *s, key k)
{
    if (key_is_direction(k)) {
        action_move(s, key_direction(k), (key_is_special(k)) ? 3 : 1);
        return;
    }

    if (key_is_mode(k)) {
        switch (k) {
            case KEY_MODE_COMMAND:
                state_set_mode(s, MODE_COMMAND);
                break;
            case KEY_MODE_AWAIT_TERRAIN:
                state_set_await(s, true);
                /* fall through */
            case KEY_MODE_TERRAIN:
                state_set_mode(s, MODE_TERRAIN);
                break;
            case KEY_MODE_AWAIT_ROAD:
                state_set_await(s, true);
                /* fall through */
            case KEY_MODE_ROAD:
                state_set_mode(s, MODE_ROAD);
                break;
            case KEY_MODE_AWAIT_RIVER:
                state_set_await(s, true);
                /* fall through */
            case KEY_MODE_RIVER:
                state_set_mode(s, MODE_RIVER);
                break;
            default:
                break;
        }
        return;
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
        state_set_await(s, false);
        if (key_is_terrain(k)) {
            action_paint_terrain(s, key_terrain(k));
        }
        state_set_mode(s, MODE_NAVIGATE);
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

    switch (k) {
        case KEY_MODE_TERRAIN:
        case KEY_ESCAPE:
            state_set_mode(s, MODE_NAVIGATE);
            return;
        default:
            break;
    }
    return;
}


void action_command(struct State *s, key k)
{
    struct Command *c = NULL;
    switch (k) {
        case KEY_ESCAPE:
            commandline_reset(state_commandline(s));
            state_set_mode(s, MODE_NAVIGATE);
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
            state_set_mode(s, MODE_NAVIGATE);
            command_destroy(c);
            return;

        /* delete character from command line */
        case KEY_BACKSPACE:
        case '\b':
        case 127:
            if (commandline_len(state_commandline(s)) <= 0) {
                commandline_reset(state_commandline(s));
                state_set_mode(s, MODE_NAVIGATE);
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
        state_set_await(s, false);
        if (key_is_direction(k)) {
            action_paint_road(s, key_direction(k));
        }
        state_set_mode(s, MODE_NAVIGATE);
        return;
    }

    if (key_is_direction(k)) {
        if (key_is_special(k)) {
            action_paint_road(s, key_direction(k));
        } else {
            action_move(s, key_direction(k), 1);
        }
    }

    switch (k) {
        case KEY_MODE_ROAD:
        case KEY_ESCAPE:
            state_set_mode(s, MODE_NAVIGATE);
            return;
        default:
            break;
    }
}


void action_river(struct State *s, key k)
{
    if (state_await(s)) {
        state_set_await(s, false);
        if (key_is_direction(k)) {
            action_paint_river(s, key_direction(k));
        }
        state_set_mode(s, MODE_NAVIGATE);
        return;
    }

    if (key_is_direction(k)) {
        if (key_is_special(k)) {
            action_drag_river(s, key_direction(k));
        } else {
            action_move(s, key_direction(k), 1);
        }
        return;
    }

    switch (k) {
        case KEY_MODE_RIVER:
        case KEY_ESCAPE:
            state_set_mode(s, MODE_NAVIGATE);
            return;
        default:
            break;
    }
}
