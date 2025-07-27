#include <ctype.h>
#include <string.h>

#include "hdr/action.h"
#include "hdr/commandline.h"
#include "hdr/interface.h"
#include "hdr/tile.h"
#include "hdr/file.h"


void action_alert(enum STATUS s, char *str)
{
    state_set_status(s);
    state_message_concat(status_string(s));
    state_message_concat(str);
}


void action_write(const char *filename)
{
    FILE *file = NULL;

    if (filename) {
        file = fopen(filename, "w");
    } else {
        if (!state_filename()) return;
        file = fopen(state_filename(), "w");
    }

    if (!file) {
        state_set_status(STATUS_ERROR_WRITE);
        return;
    }

    write_state(file);
    fclose(file);
    state_set_status(STATUS_SUCCESS_WRITE);
}


void action_edit(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file) {
        read_state(file);
        fclose(file);
        state_set_status(STATUS_SUCCESS_EDIT_OLD);
    } else {
        state_set_status(STATUS_SUCCESS_EDIT_NEW);
    }
}


void action_hint(void)
{
    ui_toggle_show(PANEL_HINT);
    ui_toggle_show(mode_panel(state_mode()));
}


void action_mode(enum MODE m)
{
    if (state_await()) {
        state_pop_mode();
        return;
    }

    if ((state_mode() == m) || (MODE_NONE == m)) {
        state_push_mode(MODE_NAVIGATE);
    } else {
        state_push_mode(m);
    }

    if (MODE_COMMAND == m) { curs_set(1); }
    else { curs_set(0); }
}


void action_move(enum DIRECTION d, int steps)
{
    struct Atlas *atlas = state_atlas();
    atlas_goto(atlas, coordinate_nshift(atlas_coordinate(atlas), d, steps));
}


void action_paint_terrain(enum TERRAIN t)
{
    struct Atlas *atlas = state_atlas();

    if (atlas_terrain(atlas) == TERRAIN_UNKNOWN) atlas_create_neighbours(atlas);
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
}


void action_paint_road(enum DIRECTION d)
{
    struct Atlas *atlas = state_atlas();
    struct Tile *tile = atlas_tile(atlas);
    struct Tile *neighbour = chart_tile(atlas_neighbour(atlas, d));

    action_move(d, 1);

    if (terrain_impassable(tile_terrain(tile))
        || terrain_impassable(tile_terrain(neighbour))) {
        return;
    }

    tile_toggle_road(tile, d);
    tile_toggle_road(neighbour, direction_opposite(d));
}


void action_paint_river(enum DIRECTION d)
{
    struct Atlas *atlas = state_atlas();
    struct Tile *tile = atlas_tile(atlas);
    struct Tile *neighbour = chart_tile(atlas_neighbour(atlas, d));

    if (terrain_impassable(tile_terrain(tile))
        || terrain_impassable(tile_terrain(neighbour))) {
        return;
    }

    tile_toggle_river(tile, d);
    tile_toggle_river(neighbour, direction_opposite(d));
}


void action_drag_river(enum DIRECTION d)
{
    struct Tile *tile = atlas_tile(state_atlas());

    if (tile_river(tile, direction_next(d)) || tile_river(tile, direction_prev(d))) {
        action_move(d, 1);

        if (tile_river(tile, direction_next(d))) {
            enum DIRECTION e = direction_prev(direction_opposite(d));
            action_paint_river(e);
        }

        if (tile_river(tile, direction_prev(d))) {
            enum DIRECTION e = direction_next(direction_opposite(d));
            action_paint_river(e);
        }

        return;
    }

    enum DIRECTION e = direction_opposite(d);
    if (tile_river(tile, direction_next(e)) || tile_river(tile, direction_prev(e))) {

        if (tile_river(tile, direction_next(e))) {
            enum DIRECTION f = direction_next(direction_next(e));
            action_paint_river(f);
        }

        if (tile_river(tile, direction_prev(e))) {
            enum DIRECTION f = direction_prev(direction_prev(e));
            action_paint_river(f);
        }

        return;
    }

    action_move(d, 1);
}


void action_paint_location(enum LOCATION t)
{
    struct Tile *tile = atlas_tile(state_atlas());

    if (terrain_impassable(tile_terrain(tile))) {
        return;
    }

    if (!tile_location(tile)) {
        atlas_create_location(state_atlas(), t);
    } else if (location_type(tile_location(tile)) == t) {
        location_set_type(tile_location(tile), LOCATION_NONE);
    } else {
        location_set_type(tile_location(tile), t);
    }
}


void action_capture(key k)
{
    (void) k;

    if (ui_is_show(PANEL_SPLASH)) ui_toggle_show(PANEL_SPLASH);
    if (STATUS_OK != state_status()) { state_set_status(STATUS_OK); }

    action_mode(MODE_NAVIGATE);
}


void action_navigate(key k)
{
    if (key_is_direction(k)) {
        action_move(key_direction(k), (key_is_special(k)) ? 3 : 1);
        return;
    }

    if (key_is_mode(k)) action_mode(key_mode(k));

    switch (k) {
        case KEY_TOGGLE_DETAIL:
            ui_toggle_show(PANEL_DETAIL);
            return;
        default:
            break;
    }
}


void action_terrain(key k)
{
    if (state_await()) {
        if (key_is_terrain(k)) {
            action_paint_terrain(key_terrain(k));
        }

        action_mode(key_mode(k));
        return;
    }

    if (key_is_terrain(k)) {
        action_paint_terrain(key_terrain(k));
        return;
    }

    if (key_is_direction(k)) {
        enum TERRAIN t = atlas_terrain(state_atlas());
        action_move(key_direction(k), 1);

        if (key_is_special(k)) {
            if (t != TERRAIN_UNKNOWN) {
                action_paint_terrain(t);
            }
        }

        return;
    }

    if (key_is_mode(k)) {
        action_mode(key_mode(k));
    }
}


void action_command(key k)
{
    switch (k) {
        case KEY_ESCAPE:
            commandline_reset();
            state_pop_mode();
            break;

        case KEY_ENTER:
        case '\n':
            commandline_parse();
            switch (commandline_command()) {
                case COMMAND_QUIT:
                    state_set_quit(true);
                    break;
                case COMMAND_WRITE:
                    action_write(commandline_data());
                    break;
                case COMMAND_EDIT:
                    action_edit(commandline_data());
                default:
                    break;
            }

            commandline_reset();
            state_pop_mode();
            break;

        case KEY_BACKSPACE:
        case '\b':
        case 127:
            if (commandline_len() <= 0) {
                commandline_reset();
                state_pop_mode();
                break;
            }

            commandline_popch();
            return;

        case 23:
            commandline_clearword();
            return;

        case 21:
            commandline_clearline();
            return;

        case KEY_LEFT:
            commandline_prev();
            return;

        case KEY_RIGHT:
            commandline_next();
            return;

        case '\t':
            return;

        default:
            commandline_putch(state_key_curr());
            return;
    }
    curs_set(0);
}


void action_road(key k)
{
    if (state_await()) {
        if (key_is_direction(k)) {
            action_paint_road(key_direction(k));
        }

        action_mode(key_mode(k));
        return;
    }

    if (key_is_direction(k)) {
        if (key_is_special(k)) {
            action_paint_road(key_direction(k));
        } else {
            action_move(key_direction(k), 1);
        }
        return;
    }

    if (key_is_mode(k)) {
        action_mode(key_mode(k));
    }
}


void action_river(key k)
{
    if (state_await()) {
        if (key_is_direction(k)) {
            action_paint_river(key_direction(k));
        }

        action_mode(key_mode(k));
        return;
    }

    if (key_is_direction(k)) {
        if (key_is_special(k)) {
            action_drag_river(key_direction(k));
        } else {
            action_move(key_direction(k), 1);
        }
        return;
    }

    if (key_is_mode(k)) {
        action_mode(key_mode(k));
        return;
    }
}


void action_location(key k)
{
    if (state_await()) {
        action_mode(key_mode(k));
    }

    if (key_is_location(k)) {
        action_paint_location(key_location(k));
    }

    if (key_is_direction(k)) {
        action_move(key_direction(k), 1);
    }

    if (key_is_mode(k)) {
        action_mode(key_mode(k));
        return;
    }
}
