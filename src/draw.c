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

void wdraw_box(WINDOW *win, int r0, int c0, int w, int h, char bg)
{
    wdraw_rectangle(win, r0, c0, w, h, bg);
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

void wdraw_feature(WINDOW *win, struct Geometry *g, int r0, int c0, enum FEATURE f)
{
    int w = geometry_tile_dw(g), h = geometry_tile_dh(g);
    int r = r0 - h/2, c = c0 - w/2;
    switch (f) {
        case FEATURE_SETTLEMENT:
            wdraw_box(win, r--, c++, w, h, ' ');
            w -=2;
            while (w > 0) {
                mvwhline(win, r--, c, '=', w);
                w -= 4;
                c += 2;
            }
        default:
            break;
    }
}

/*
*     DRAW 02 - Panels
 */

void wdraw_panel(WINDOW *win, struct Panel *p)
{
    int r = panel_row(p), c = panel_col(p);
    int w = panel_width(p), h = panel_height(p);
    wdraw_box(win, r, c, w, h, ' ');

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

void wdraw_tile(WINDOW *win, struct Geometry *g, struct Tile *tile, int r0, int c0)
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

    wdraw_feature(win, g, r0, c0, tile_feature(tile));
}

void wdraw_chart(WINDOW *win,
                 struct Geometry *g, struct Chart *chart, struct Coordinate *o)
{
    /* recurse down the chart to find the tiles at the bottom and print them
     * send the right integer coordinates */
    if (!chart) {
        return;
    }
    if (chart_children(chart)) {
        for (int i = 0; i < NUM_CHILDREN; i++) {
            wdraw_chart(win, g, chart_child(chart, i), o);
        }
    }
    if (!chart_tile(chart)) {
        return;
    }

    struct Coordinate *c = chart_coordinate(chart);
    int dp = coordinate_p(c) - coordinate_p(o), dq = coordinate_q(c) - coordinate_q(o);

    int dr = 3*dq*geometry_tile_dh(g), dc = (2*dp + dq)*geometry_tile_dw(g);

    wdraw_tile(win, g, chart_tile(chart), geometry_rmid(g) + dr, geometry_cmid(g) + dc);
}

void wdraw_atlas_at(WINDOW *win,
                    struct Geometry *g, struct Atlas *atlas, struct Coordinate *c)
{
    struct Chart *chart = atlas_find(atlas, c);
    if (!chart && coordinate_m(c)) {
        struct Coordinate *ch = coordinate_create_origin();
        for (int i = 0; i < NUM_CHILDREN; i++) {
            coordinate_child(c, i, ch);
            wdraw_atlas_at(win, g, atlas, ch);
        }
        coordinate_destroy(ch);
        return;
    }
    wdraw_chart(win, g, chart, atlas_coordinate(atlas));
}

void wdraw_atlas(WINDOW *win, struct Geometry *g, struct Atlas *atlas)
{
    /* find the smallest coordinate that is guaranteed to contain the window
     * send this coordinate to wdraw_atlas_at
     * */

    struct Chart *centre = atlas_curr(atlas);
    struct Coordinate *c = chart_coordinate(centre);

    /* TODO move this calculation to Geometry struct */
    int n_hor = round(geometry_cols(g)/(2.00f*geometry_tile_dw(g))) + 1,
        n_ver = round(geometry_rows(g)/(1.50f*geometry_tile_dh(g))) + 1;

    struct Coordinate *c_E = coordinate_duplicate(c),
        *c_W = coordinate_duplicate(c),
        *c_N = coordinate_duplicate(c), *c_S = coordinate_duplicate(c);

    coordinate_nshift(c_W, DIRECTION_WW, n_hor / 2);
    coordinate_nshift(c_E, DIRECTION_EE, n_hor / 2);
    coordinate_nshift(c_N, DIRECTION_NW, n_ver / 4);
    coordinate_nshift(c_N, DIRECTION_NE, n_ver / 4);
    coordinate_nshift(c_S, DIRECTION_SW, n_ver / 4);
    coordinate_nshift(c_S, DIRECTION_SE, n_ver / 4);

    struct Coordinate *tmp1 = coordinate_create_ancestor(c_E, c_W);
    struct Coordinate *tmp2 = coordinate_create_ancestor(c_N, c_S);
    struct Coordinate *a = coordinate_create_ancestor(tmp1, tmp2);

    wdraw_atlas_at(win, g, atlas, a);

    coordinate_destroy(c_E);
    coordinate_destroy(c_W);
    coordinate_destroy(c_N);
    coordinate_destroy(c_S);
    coordinate_destroy(tmp1);
    coordinate_destroy(tmp2);
    coordinate_destroy(a);
}

/*
*     DRAW 04 - Core stuff
 */

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
            addstr(commandline_str(state_commandline(s)));
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
