#include <ctype.h>
#include <string.h>

#include "include/action.h"
#include "include/commandline.h"
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


void action_capture(struct State *s, key k)
{
    if (k != KEY_ENTER && k != '\n') {
        return;
    }

    if (ui_show(state_ui(s), PANEL_SPLASH)) {
        ui_toggle(state_ui(s), PANEL_SPLASH);
    }

    state_set_mode(s, INPUT_MODE_NAVIGATE);
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
                state_set_mode(s, INPUT_MODE_COMMAND);
                break;
            case KEY_MODE_AWAIT_TERRAIN:
                state_set_await(s, true);
                /* fall through */
            case KEY_MODE_TERRAIN:
                state_set_mode(s, INPUT_MODE_TERRAIN);
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
            action_paint(s, key_terrain(k));
        }
        state_set_mode(s, INPUT_MODE_NAVIGATE);
        return;
    }

    if (key_is_terrain(k)) {
        action_paint(s, key_terrain(k));
        return;
    }

    if (key_is_direction(k)) {
        enum DIRECTION d = key_direction(k);
        enum TERRAIN t = map_curr_terrain(state_map(s));
        action_move(s, d, 1);

        if (key_is_special(k)) {
            if (t != TERRAIN_UNKNOWN) {
                action_paint(s, t);
            }
        }
    }

    switch (k) {
        case KEY_MODE_TERRAIN:
            state_set_mode(s, INPUT_MODE_NAVIGATE);
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

        /* send command to be parsed */
        case KEY_ENTER:
        case 27:
        case '\n':
            c = commandline_parse(state_commandline(s));
            switch (command_type(c)) {
                case COMMAND_TYPE_QUIT:
                    state_set_quit(s, true);
                default:
                    break;
            }
            commandline_reset(state_commandline(s));
            state_set_mode(s, INPUT_MODE_NAVIGATE);
            return;

        /* delete character from command line */
        case KEY_BACKSPACE:
        case '\b':
        case 127:
            if (commandline_len(state_commandline(s)) <= 0) {
                commandline_reset(state_commandline(s));
                state_set_mode(s, INPUT_MODE_NAVIGATE);
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
