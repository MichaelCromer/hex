#include <stdlib.h>
#include <string.h>

#include "include/atlas.h"
#include "include/interface.h"
#include "include/state.h"


struct Panel {
    int r0, c0;
    unsigned int w, h;
    size_t len;
    char **lines;
};


struct Panel *panel_create(size_t len)
{
    struct Panel *panel = malloc(sizeof(struct Panel));
    if (!panel) return NULL;

    panel->r0 = 0;
    panel->c0 = 0;
    panel->w = 0;
    panel->h = 0;
    panel->len = len;

    panel->lines = malloc(len * sizeof(char *));
    if (!panel->lines) {
        free(panel);
        panel = NULL;
        return NULL;
    }

    for (size_t i = 0; i < len; i++) {
        panel->lines[i] = NULL;
    }

    return panel;
}


void panel_destroy(struct Panel *p)
{
    if (!p) return;

    for (size_t i = 0; i < p->len; i++) {
        if (p->lines[i]) free(p->lines[i]);
    }
    free(p->lines);
    free(p);
}


int panel_row(struct Panel *p) { return p->r0; }
int panel_col(struct Panel *p) { return p->c0; }
unsigned int panel_width(struct Panel *p) { return p->w; }
unsigned int panel_height(struct Panel *p) { return p->h; }
size_t panel_len(struct Panel *p) { return p->len; }


char *panel_line(struct Panel *p, size_t i)
{
    if (p && (i < p->len)) return p->lines[i];
    return NULL;
}


void panel_fit_line(struct Panel *panel, size_t i , size_t L)
{
    if ((L + 4) > panel->w) { panel->w = L + 4; }   /* +4 to fit border and pad */
    if ((i + 5) > panel->h) { panel->h = i + 5; }   /* +5 to fit border and pad */
}


void panel_add_line(struct Panel *panel, size_t i, const char *line)
{
    if ((i >= panel->len) || (panel->lines[i])) return;
    panel->lines[i] = strdup(line);
    panel_fit_line(panel, i, strlen(line));
}


void panel_remove_line(struct Panel *panel, size_t i)
{
    if ((i >= panel->len) || (!panel->lines[i])) return;

    free(panel->lines[i]);
    panel->lines[i] = NULL;

    panel->h = 0;
    panel->w = 0;
    for (size_t j = 0; j < panel->len; j++) {
        if (!panel->lines[j]) continue;
        panel_fit_line(panel, j, strlen(panel->lines[j]));
    }

    return;
}


void panel_set_rc(struct Panel *p, int r, int c)
{
    p->r0 = r;
    p->c0 = c;
}


void panel_centre(struct Panel *p, int H, int W)
{
    p->r0 = H - p->h/2;
    p->c0 = W - p->w/2;
}


struct Panel *panel[NUM_UI_PANELS] = { 0 };
bool show[NUM_UI_PANELS] = { 0 };


void ui_initialise(void)
{
    struct Panel *splash = panel_create(6);
    panel_add_line(splash, 0, "WELCOME TO HEX                                 ");
    panel_add_line(splash, 1, "                                               ");
    panel_add_line(splash, 2, "  A ncurses hex map editor with modal editing. ");
    panel_add_line(splash, 3, "  Press <?> while editing for contextual help. ");
    panel_add_line(splash, 4, "                                               ");
    panel_add_line(splash, 5, "                              Close: <Enter>   ");
    panel[PANEL_SPLASH] = splash;

    struct Panel *detail = panel_create(3);
    panel_add_line(detail, 0, "Currently at: ");
    panel_add_line(detail, 1, "    (p, q, r)");
    panel_add_line(detail, 2, "    TERRAIN: NONE");
    panel[PANEL_DETAIL] = detail;

    struct Panel *hint = panel_create(12);
    panel_add_line(hint, 0, "MODES AND OPTIONS  ");
    panel_add_line(hint, 1, "                   ");
    panel_add_line(hint, 2, "  T/t   Terrain    ");
    panel_add_line(hint, 3, "  R/r   Roads      ");
    panel_add_line(hint, 4, "  Y/y   Rivers     ");
    panel_add_line(hint, 5, "  F/f   Locations  ");
    panel_add_line(hint, 6, "                   ");
    panel_add_line(hint, 7, "    :   Command    ");
    panel_add_line(hint, 8, "    ?   Help       ");
    panel[PANEL_HINT] = hint;

    struct Panel *navigate = panel_create(10);
    panel_add_line(navigate, 0, "MODE: NAVIGATION   ");
    panel_add_line(navigate, 1, "                   ");
    panel_add_line(navigate, 2, "      u   i        ");
    panel_add_line(navigate, 3, "       \\ /        ");
    panel_add_line(navigate, 4, "    h -   - k      ");
    panel_add_line(navigate, 5, "       / \\        ");
    panel_add_line(navigate, 6, "      n   m        ");
    panel_add_line(navigate, 7, "                   ");
    panel_add_line(navigate, 8, "  uihknm : 1 step  ");
    panel_add_line(navigate, 9, "  UIHKNM : 3 step  ");
    panel[PANEL_NAVIGATE] = navigate;

    struct Panel *terrain = panel_create(14);
    panel_add_line(terrain, 0,  "MODE: TERRAIN      ");
    panel_add_line(terrain, 1,  "                   ");
    panel_add_line(terrain, 2,  "  uihknm : move    ");
    panel_add_line(terrain, 3,  "  UIHKNM : drag    ");
    panel_add_line(terrain, 4,  "                   ");
    panel_add_line(terrain, 5,  "    q : water      ");
    panel_add_line(terrain, 6,  "    w : mountain   ");
    panel_add_line(terrain, 7,  "    e : hills      ");
    panel_add_line(terrain, 8,  "    a : plains     ");
    panel_add_line(terrain, 9,  "    s : forest     ");
    panel_add_line(terrain, 10, "    d : swamp      ");
    panel_add_line(terrain, 11, "    z : desert     ");
    panel_add_line(terrain, 12, "    x : jungle     ");
    panel_add_line(terrain, 13, "    c : tundra     ");
    panel[PANEL_TERRAIN] = terrain;

    struct Panel *await_terrain = panel_create(11);
    panel_add_line(await_terrain, 0,  "MODE: (terrain)    ");
    panel_add_line(await_terrain, 1,  "                   ");
    panel_add_line(await_terrain, 2,  "    q : water      ");
    panel_add_line(await_terrain, 3,  "    w : mountain   ");
    panel_add_line(await_terrain, 4,  "    e : hills      ");
    panel_add_line(await_terrain, 5,  "    a : plains     ");
    panel_add_line(await_terrain, 6,  "    s : forest     ");
    panel_add_line(await_terrain, 7,  "    d : swamp      ");
    panel_add_line(await_terrain, 8,  "    z : desert     ");
    panel_add_line(await_terrain, 9,  "    x : jungle     ");
    panel_add_line(await_terrain, 10, "    c : tundra     ");
    panel[PANEL_AWAIT_TERRAIN] = await_terrain;

    struct Panel *road = panel_create(9);
    panel_add_line(road, 0,  "MODE: ROAD         ");
    panel_add_line(road, 1,  "                   ");
    panel_add_line(road, 2,  "  uihknm : move    ");
    panel_add_line(road, 3,  "  UIHKNM : drag    ");
    panel_add_line(road, 4,  "                   ");
    panel_add_line(road, 5,  "  Drag over once   ");
    panel_add_line(road, 6,  "  to place road,   ");
    panel_add_line(road, 7,  "  drag a second    ");
    panel_add_line(road, 8,  "  time to remove   ");
    panel[PANEL_ROAD] = road;

    struct Panel *await_road = panel_create(9);
    panel_add_line(await_road, 0,  "MODE: (road)       ");
    panel_add_line(await_road, 1,  "                   ");
    panel_add_line(await_road, 2,  "  uihknm : paint   ");
    panel[PANEL_AWAIT_ROAD] = await_road;

    struct Panel *river = panel_create(12);
    panel_add_line(river, 0,  "MODE: RIVER        ");
    panel_add_line(river, 1,  "                   ");
    panel_add_line(river, 2,  "  uihknm : move    ");
    panel_add_line(river, 3,  "  UIHKNM : drag    ");
    panel_add_line(river, 4,  "                   ");
    panel_add_line(river, 5,  "  Can only drag    ");
    panel_add_line(river, 6,  "  existing river   ");
    panel_add_line(river, 7,  "  placed via the   ");
    panel_add_line(river, 8,  "  (river) mode     ");
    panel[PANEL_RIVER] = river;

    struct Panel *await_river = panel_create(12);
    panel_add_line(await_river, 0,  "MODE: (river)      ");
    panel_add_line(await_river, 1,  "                   ");
    panel_add_line(await_river, 2,  "  uihknm : paint   ");
    panel[PANEL_AWAIT_RIVER] = await_river;

    struct Panel *location = panel_create(12);
    panel_add_line(location, 0,  "MODE: LOCATION     ");
    panel_add_line(location, 1,  "                   ");
    panel_add_line(location, 2,  "  uihknm : move    ");
    panel_add_line(location, 3,  "                   ");
    panel_add_line(location, 4,  "    a : interest   ");
    panel_add_line(location, 5,  "    s : settlement ");
    panel_add_line(location, 6,  "    d : dungeon    ");
    panel[PANEL_LOCATION] = location;

    struct Panel *await_location = panel_create(12);
    panel_add_line(await_location, 0,  "MODE: (location)   ");
    panel_add_line(await_location, 1,  "                   ");
    panel_add_line(await_location, 2,  "    a : feature    ");
    panel_add_line(await_location, 3,  "    s : settlement ");
    panel_add_line(await_location, 4,  "    d : dungeon    ");
    panel[PANEL_AWAIT_LOCATION] = await_location;

    struct Panel *command = panel_create(12);
    panel_add_line(command, 0,  "MODE: COMMAND      ");
    panel_add_line(command, 1,  "                   ");
    panel_add_line(command, 2,  "    q[uit]         ");
    panel_add_line(command, 3,  "    w[rite] <path> ");
    panel_add_line(command, 4,  "    e[dit]  <path> ");
    panel[PANEL_COMMAND] = command;

    panel_centre(panel[PANEL_SPLASH], geometry_rmid(), geometry_cmid());

    panel_set_rc(
        panel[PANEL_HINT],
        1,
        geometry_cols() - panel_width(panel[PANEL_HINT]) - 1
    );

    for (enum UI_PANEL p = PANEL_NAVIGATE; p < PANEL_NONE; p++) {
        panel_set_rc(
            panel[p],
            panel_height(panel[PANEL_HINT]),
            geometry_cols() - panel_width(panel[p]) - 1
        );
    }

    show[PANEL_SPLASH] = true;
}


void ui_update(struct State *state)
{
    /*
     *  tile detail panel
     */
    struct Atlas *atlas = state_atlas(state);
    struct Chart *chart = atlas_curr(atlas);
    struct Panel *detail = panel[PANEL_DETAIL];

    char buf[32];

    memset(buf, 0, 32);
    panel_remove_line(detail, 1);
    snprintf(buf, 32, "  (%d, %d, %d)", chart_p(chart), chart_q(chart), chart_r(chart));
    panel_add_line(detail, 1, buf);

    memset(buf, 0, 32);
    panel_remove_line(detail, 2);
    snprintf(buf, 32, "  Terrain: %s", terrain_name(atlas_terrain(atlas)));
    panel_add_line(detail, 2, buf);

    /*
     * help/hint panels
     */
    if (!show[PANEL_HINT]) return;
    for (enum UI_PANEL p = PANEL_NAVIGATE; p < PANEL_NONE; p++) show[p] = false;
    enum UI_PANEL curr = mode_panel(state_mode(state));
    if (curr == PANEL_NONE) return;
    show[curr] = true;
}


void ui_toggle_show(enum UI_PANEL p)
{
    if (p == PANEL_NONE) return;
    show[p] = !(show[p]);
}


bool ui_is_show(enum UI_PANEL p)
{
    if (p == PANEL_NONE) return false;
    return show[p];
}


struct Panel *ui_panel(enum UI_PANEL p)
{
    if (p == PANEL_NONE) return NULL;
    return panel[p];
}
