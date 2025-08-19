#include <limits.h>
#include <math.h>
#include <ncurses.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "hdr/action.h"
#include "hdr/atlas.h"
#include "hdr/enum.h"
#include "hdr/geometry.h"
#include "hdr/interface.h"
#include "hdr/key.h"
#include "hdr/state.h"


bool quit = false;
bool reticule = true;
bool modified = false;
char *filename = NULL;
char *cwd = NULL;
enum STATUS status = STATUS_OK;
char *message = NULL;
key key_curr = 0;
enum MODE mode_curr = MODE_NONE;
enum MODE mode_prev = MODE_NONE;
WINDOW *window = NULL;
struct Atlas *atlas = NULL;


bool state_quit(void) { return quit; }
bool state_reticule(void) { return reticule; }
bool state_modified(void) { return modified; }
const char *state_filename(void) { return filename; }
const char *state_cwd(void) { return cwd; }
key state_key_curr(void) { return key_curr; }
bool state_await(void) { return mode_is_await(mode_curr); }
WINDOW *state_window(void) { return window; }
enum MODE state_mode(void) { return mode_curr; }
enum MODE state_lastmode(void) { return mode_prev; }
enum STATUS state_status(void) { return status; }
struct Atlas *state_atlas(void) { return atlas; }
char *state_message(void) { return message; }


void state_initialise(WINDOW *win, const char *str_filename)
{
    window = win;

    geometry_initialise(GEOMETRY_DEFAULT_SCALE, GEOMETRY_DEFAULT_ASPECT, win);
    ui_initialise();

    atlas = atlas_create();
    atlas_initialise(atlas);

    char cwd[PATH_MAX] = { 0 };
    getcwd(cwd, PATH_MAX);
    state_set_cwd(cwd);

    state_push_mode(MODE_NAVIGATE);
    state_push_mode(MODE_NAVIGATE);
    if (!str_filename) {
        state_push_mode(MODE_CAPTURE);
        ui_toggle_show(PANEL_SPLASH);
    } else {
        state_set_filename(str_filename);
        action_edit(filename);
    }
}


void state_update(void)
{
    key k = wgetch(state_window());
    key_curr = k;

    if ((MODE_COMMAND != state_mode()) && (KEY_TOGGLE_HELP == k)) {
        action_hint();
        return;
    }

    if (STATUS_OK != status) {
        action_capture(k);
        return;
    }

    switch (state_mode()) {
        case MODE_CAPTURE:
            action_capture(k);
            break;
        case MODE_NAVIGATE:
            action_navigate(k);
            break;
        case MODE_TERRAIN:
        case MODE_AWAIT_TERRAIN:
            action_terrain(k);
            break;
        case MODE_COMMAND:
            action_command(k);
            break;
        case MODE_ROAD:
        case MODE_AWAIT_ROAD:
            action_road(k);
            break;
        case MODE_AWAIT_RIVER:
        case MODE_RIVER:
            action_river(k);
            break;
        case MODE_AWAIT_LOCATION:
        case MODE_LOCATION:
            action_location(k);
            break;
        case MODE_NONE:
        default:
            break;
    }

    ui_update();
    geometry_calculate_viewpoint(atlas_coordinate(state_atlas()));
}


void state_set_quit(bool q) { quit = q; }
void state_set_modified(bool m) { modified = m; }
void state_set_status(enum STATUS s) { status = s; }


void state_set_cwd(const char *str_cwd)
{
    if (!str_cwd || cwd) return;
    cwd = strdup(str_cwd);
}


void state_set_atlas(struct Atlas *a)
{
    if (atlas) return;
    atlas = a;
}


void state_set_filename(const char *str)
{
    if (filename) free(filename);
    filename = strdup(str);
}


void state_clear_atlas(void)
{
    if (!atlas) return;
    atlas_destroy(atlas);
    atlas = NULL;
}


void state_clear_filename(void)
{
    if (!filename) return;
    free(filename);
    filename = NULL;
}


void state_clear_message(void)
{
    if (message) free(message);
    message = NULL;
}


void state_deinitialise(void)
{
    quit = false;
    modified = false;
    reticule = true;

    key_curr = 0;
    mode_curr = MODE_NONE;
    mode_prev = MODE_NONE;
    window = NULL;

    state_clear_atlas();
    state_clear_filename();
}


void state_push_mode(enum MODE mode)
{
    mode_prev = mode_curr;
    mode_curr = mode;
}


void state_pop_mode(void)
{
    mode_curr = mode_prev;
}


void state_message_concat(const char *str)
{
    if (!str) return;
    if (!message) {
        message = strdup(str);
        return;
    }
    char *tmp = realloc(message, strlen(message) + strlen(str) + 1);
    if (!tmp) return;
    message = tmp;
    strcat(message, str);
}


void state_message_clear(void)
{
    if (!message) return;
    free(message);
    message = NULL;
}
