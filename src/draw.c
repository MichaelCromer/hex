#include <ncurses.h>
#include <math.h>

#include "include/commandline.h"
#include "include/draw.h"
#include "include/enum.h"
#include "include/geometry.h"
#include "include/atlas.h"
#include "include/interface.h"
#include "include/panel.h"
#include "include/state.h"
#include "include/tile.h"

/*
*     DRAW 01 - Basic shapes
 */

void wdraw_border(WINDOW *win, int r0, int c0, int w, int h)
{
    mvwhline(win, r0, c0, ACS_HLINE, w - 1);
    mvwvline(win, r0, c0, ACS_VLINE, h - 1);
    mvwvline(win, r0, c0 + w - 1, ACS_VLINE, h - 1);
    mvwhline(win, r0 + h - 1, c0, ACS_HLINE, w - 1);

    mvwaddch(win, r0, c0, ACS_PLUS);
    mvwaddch(win, r0, c0 + w - 1, ACS_PLUS);
    mvwaddch(win, r0 + h - 1, c0, ACS_PLUS);
    mvwaddch(win, r0 + h - 1, c0 + w - 1, ACS_PLUS);
}

void wdraw_rectangle(WINDOW *win, int r0, int c0, int w, int h, char bg)
{
    for (int r = 0; r < h; r++) {
        mvwhline(win, r0 + r, c0, bg, w);
    }
}

void wdraw_box(WINDOW *win, int r0, int c0, int w, int h)
{
    wdraw_rectangle(win, r0, c0, w, h, ' ');
    wdraw_border(win, r0, c0, w, h);
}

void wdraw_line(WINDOW *win, int r0, int c0, int r1, int c1, char ch)
{
    int R = (r1 - r0), C = (c1 - c0);
    float L = (float)sqrt(R*R + C*C);
    if (L < 0.01f) {
        return;
    }
    float dr = (float)(R / L), dc = (float)(C / L);
    float r = 0, c = 0;
    for (int i = 0; i <= L; i++) {
        mvwaddch(win, r0 + round(r), c0 + round(c), ch);
        r += dr;
        c += dc;
    }
}

/*
*     DRAW 02 - Panels
 */

void wdraw_panel(WINDOW *win, struct Panel *p)
{
    int r = panel_row(p), c = panel_col(p);
    int w = panel_width(p), h = panel_height(p);
    wdraw_box(win, r, c, w, h);

    for (int i = 0; i < panel_len(p); i++) {
        char *line = panel_line(p, i);
        if (!line) {
            continue;
        }
        mvwprintw(win, r + 2 + i, c + 2, "%s", line);
    }
}

/*
*     DRAW 03 - Charts and terrain
 */

void wdraw_road(WINDOW *win, struct Geometry *g, int r, int c, enum DIRECTION d)
{
    int dr = 0, dc = 0, dw = geometry_tile_dw(g), dh = geometry_tile_dh(g);

    switch (d) {
        case DIRECTION_EE:
            dc = 2 * dw;
            break;
        case DIRECTION_NE:
            dr = -3 * dh;
            dc = dw;
            break;
        case DIRECTION_NW:
            dr = -3 * dh;
            dc = -1 * dw;
            break;
        case DIRECTION_WW:
            dc = -2 * dw;
            break;
        case DIRECTION_SW:
            dr = 3 * dh;
            dc = -1 * dw;
            break;
        case DIRECTION_SE:
            dr = 3 * dh;
            dc = dw;
            break;
        default:
            break;
    }

    wdraw_line(win, r, c, r + dr, c + dc, '#');
}

void wdraw_river(WINDOW *win, struct Geometry *g, int r, int c, enum DIRECTION d)
{
    int c0 = 0, r0 = 0, c1 = 0, r1 = 0,
        dw = geometry_tile_dw(g), dh = geometry_tile_dh(g);

    switch (d) {
        case DIRECTION_EE:
            r0 = r + dh;
            c0 = c + dw;
            r1 = r - dh;
            c1 = c + dw;
            break;
        case DIRECTION_NE:
            r0 = r - dh;
            c0 = c + dw;
            r1 = r - 2*dh;
            c1 = c;
            break;
        case DIRECTION_NW:
            r0 = r - 2*dh;
            c0 = c;
            r1 = r - dh;
            c1 = c - dw;
            break;
        case DIRECTION_WW:
            r0 = r + dh;
            c0 = c - dw;
            r1 = r - dh;
            c1 = c - dw;
            break;
        case DIRECTION_SW:
            r0 = r + dh;
            c0 = c - dw;
            r1 = r + 2*dh;
            c1 = c;
            break;
        case DIRECTION_SE:
            r0 = r + 2*dh;
            c0 = c;
            r1 = r + dh;
            c1 = c + dw;
            break;
        default:
            break;
    }
    wdraw_line(win, r0, c0, r1, c1, '~');
}

void wdraw_settlement(WINDOW *win, struct Geometry *g, int r0, int c0)
{
    int w = geometry_tile_dw(g) - 1, h = geometry_tile_dh(g);
    int r = r0 - h/2 + 1, c = c0 - w/2;

    if (w < 3) {
        return;
    }

    wdraw_rectangle(win, r, c, w, h, ' ');

    /* roof */
    mvwhline(win, r - 1, c + 1, '^', w - 1);
    mvwhline(win, r - 2, c + 2, '^', w - 3);

    /* walls */
    mvwvline(win, r, c, '|', h);
    mvwvline(win, r, c + w, '|', h);

    /* floor */
    mvwhline(win, r + h - 1, c + 1, '_', w - 1);

    /* corners */
    mvwaddch(win, r + h - 1, c, '+');
    mvwaddch(win, r + h - 1, c + w, '+');
    mvwaddch(win, r - 1, c, '+');
    mvwaddch(win, r - 1, c + w, '+');
}

void wdraw_feature(WINDOW *win, struct Geometry *g, int r0, int c0)
{
    int w = geometry_tile_dw(g), h = geometry_tile_dh(g);
    int r = r0 - h/2 - 1, c = c0 - w/2 - 1;

    if (w < 3) {
        return;
    }

    wdraw_rectangle(win, r, c, 2, 2, '#');
    wdraw_rectangle(win, r + h, c, 2, 2, '#');
    wdraw_rectangle(win, r, c + w, 2, 2, '#');
    wdraw_rectangle(win, r + h, c + w, 2, 2, '#');
    wdraw_box(win, r0 - 1, c0 - 1, 3, 3);
}

void wdraw_dungeon(WINDOW *win, struct Geometry *g, int r0, int c0)
{
    int w = geometry_tile_dw(g) - 1, h = geometry_tile_dh(g);
    int r = r0 - h/2 + 1, c = c0 - w/2;

    if (w < 3) {
        return;
    }

    wdraw_rectangle(win, r, c, w, h, ' ');
    mvwhline(win, r, c, '#', w);
    mvwhline(win, r - 1, c + 1, '#', w - 1);

    mvwvline(win, r, c, '#', h);
    mvwvline(win, r, c + 1, '#', h);

    mvwvline(win, r, c + w, '#', h);
    mvwvline(win, r, c + w - 1, '#', h);
}

void wdraw_tile_location(WINDOW *win, struct Geometry *g, struct Tile *tile, int r0,
                         int c0)
{
    if (!tile || !tile_location(tile)) {
        return;
    }

    enum LOCATION t = location_type(tile_location(tile));
    switch (t) {
        case LOCATION_SETTLEMENT:
            wdraw_settlement(win, g, r0, c0);
            break;
        case LOCATION_FEATURE:
            wdraw_feature(win, g, r0, c0);
            break;
        case LOCATION_DUNGEON:
            wdraw_dungeon(win, g, r0, c0);
            break;
        default:
            break;
    }
}

void wdraw_tile_terrain(WINDOW *win, struct Geometry *g, struct Tile *tile, int r0,
                        int c0)
{
    int w = geometry_tile_dw(g), h = geometry_tile_dh(g);

    attron(COLOR_PAIR(terrain_colour(tile_terrain(tile))));
    for (int c = -w; c <= w; c++) {
        int dh = (c < 0)
            ? floor((w + c)*geometry_slope(g))
            : floor((w - c)*geometry_slope(g));
        for (int r = -(h + dh); r <= (h + dh); r++) {
            mvwaddch(win, r0 + r, c0 + c, tile_getch(tile, c, r));
        }
    }
    attroff(COLOR_PAIR(terrain_colour(tile_terrain(tile))));

    attron(COLOR_PAIR(COLOR_YELLOW));
    for (int i = 0; i < NUM_DIRECTIONS; i++) {
        if (tile_road(tile, i)) {
            wdraw_road(win, g, r0, c0, i);
        }
    }
    attroff(COLOR_PAIR(COLOR_YELLOW));

    attron(COLOR_PAIR(COLOR_CYAN));
    for (int i = 0; i < NUM_DIRECTIONS; i++) {
        if (tile_river(tile, i)) {
            wdraw_river(win, g, r0, c0, i);
        }
    }
    attroff(COLOR_PAIR(COLOR_CYAN));
}

void wdraw_chart_with(
    WINDOW *win,
    struct Geometry *g,
    struct Chart *chart,
    struct Coordinate v,
    struct Coordinate o,
    void (*wdraw_tile)(WINDOW *, struct Geometry *, struct Tile *, int, int)
)
{
    if (!chart || !wdraw_tile) return;

    /* don't bother if this chart doesn't overlap with the target viewpoint */
    if (!coordinate_related(chart_coordinate(chart), v)) return;

    /* recurse */
    if (chart_children(chart)) {
        for (int i = 0; i < NUM_CHILDREN; i++) {
            wdraw_chart_with(win, g, chart_child(chart, i), v, o, wdraw_tile);
        }
    }

    /* draw own tile */
    if (chart_tile(chart)) {
        int r0 = geometry_rmid(g), c0 = geometry_cmid(g);
        int dp = coordinate_p(chart_coordinate(chart)) - coordinate_p(o),
            dq = coordinate_q(chart_coordinate(chart)) - coordinate_q(o);

        int r = r0 + 3*dq*geometry_tile_dh(g), c =
            c0 + (2*dp + dq)*geometry_tile_dw(g);
        wdraw_tile(win, g, chart_tile(chart), r, c);
    }
}

void wdraw_atlas(WINDOW *win, struct Geometry *g, struct Atlas *atlas)
{
    struct Coordinate v = atlas_viewpoint(atlas);
    struct Coordinate o = atlas_coordinate(atlas);

    wdraw_chart_with(win, g, atlas_root(atlas), v, o, wdraw_tile_terrain);
    wdraw_chart_with(win, g, atlas_root(atlas), v, o, wdraw_tile_location);
}

void wdraw_reticule(WINDOW *win, struct Geometry *g)
{
    char x = '#';
    int w = geometry_tile_dw(g), h = geometry_tile_dh(g);
    int r = geometry_rmid(g), c = geometry_cmid(g);

    attron(COLOR_PAIR(COLOR_RED));

    mvwaddch(win, r + h, c + w, x);
    mvwaddch(win, r - h, c + w, x);
    mvwaddch(win, r + h, c - w, x);
    mvwaddch(win, r - h, c - w, x);
    mvwaddch(win, r + 2*h, c, x);
    mvwaddch(win, r - 2*h, c, x);

    attroff(COLOR_PAIR(COLOR_RED));
    return;
}

void wdraw_ui(WINDOW *win, struct UserInterface *ui)
{
    for (int p = 0; p < NUM_UI_PANELS; p++) {
        if (ui_show(ui, p)) {
            wdraw_panel(win, ui_panel(ui, p));
        }
    }
}

void wdraw_statusline(WINDOW *win, struct State *s)
{
    int r0 = geometry_rows(state_geometry(s)) - 1,
        c0 = 0, w = geometry_cols(state_geometry(s)) - 1;

    mvwhline(win, r0, c0, ' ', w);
    wmove(win, r0, c0 + 1);

    if (state_await(s)) {
        attron(COLOR_PAIR(mode_colour(state_lastmode(s))));
        waddstr(win, mode_name(state_lastmode(s)));
        attroff(COLOR_PAIR(mode_colour(state_lastmode(s))));
        addch(' ');
    }

    attron(COLOR_PAIR(mode_colour(state_mode(s))));
    waddstr(win, mode_name(state_mode(s)));
    attroff(COLOR_PAIR(mode_colour(state_mode(s))));

    switch (state_mode(s)) {
        case MODE_COMMAND:
            addch(' ');
            addch(':');
            addstr(commandline_str());
            break;
        case MODE_TERRAIN:
        case MODE_AWAIT_TERRAIN:
            addch(' ');
            attron(COLOR_PAIR(COLOR_YELLOW));
            for (const char *c = terrain_statusline(); *c != '\0'; c++) {
                if (*c == ' ') {
                    attron(COLOR_PAIR(COLOR_YELLOW));
                } else if (*c == ':') {
                    attroff(COLOR_PAIR(COLOR_YELLOW));
                }
                addch(*c);
            }
            break;
        default:
            break;
    }
}

void draw_state(struct State *s)
{
    wdraw_atlas(state_window(s), state_geometry(s), state_atlas(s));
    wdraw_reticule(state_window(s), state_geometry(s));
    wdraw_ui(state_window(s), state_ui(s));
    wdraw_statusline(state_window(s), s);
    return;
}
