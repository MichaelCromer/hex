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
 *      DRAW 01 - Basic shapes
 */

void wdraw_border(WINDOW *win, int r0, int c0, int w, int h)
{
    mvwhline(win, r0,     c0,     ACS_HLINE, w-1);
    mvwvline(win, r0,     c0,     ACS_VLINE, h-1);
    mvwvline(win, r0,     c0+w-1, ACS_VLINE, h-1);
    mvwhline(win, r0+h-1, c0,     ACS_HLINE, w-1);

    mvwaddch(win, r0,     c0,     ACS_PLUS);
    mvwaddch(win, r0,     c0+w-1, ACS_PLUS);
    mvwaddch(win, r0+h-1, c0,     ACS_PLUS);
    mvwaddch(win, r0+h-1, c0+w-1, ACS_PLUS);
}


void wdraw_rectangle(WINDOW *win, int r0, int c0, int w, int h, char bg)
{
    for (int r=0; r<h; r++) {
        mvwhline(win, r0+r, c0, bg, w);
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
    if (!L) {
        return;
    }
    float dr = (float)(R / L), dc = (float)(C / L);
    float r = 0, c = 0;
    for (int i = 0; i <= L+1; i++) {
        mvwaddch(win, r0 + round(r), c0 + round(c), ch);
        r += dr;
        c += dc;
    }
}


void wdraw_road(WINDOW *win, struct Geometry *g, int r0, int c0, enum DIRECTION d)
{
    float dr = 0, dc = 0,
          Sr = geometry_scale(g) / 2,
          Sc = geometry_scale(g) * geometry_aspect(g) / 2;

    switch (d) {
        case DIRECTION_EE:
            dr = ROOT3 * Sr;
            break;
        case DIRECTION_NE:
            dr = ROOT3 * Sr / 2;
            dc = -3 * Sc / 2;
            break;
        case DIRECTION_NW:
            dr = -1 * ROOT3 * Sr / 2;
            dc = -3 * Sc / 2;
            break;
        case DIRECTION_WW:
            dr = -1 * ROOT3 * Sr;
            break;
        case DIRECTION_SW:
            dr = -1 * ROOT3 * Sr / 2;
            dc = 3 * Sc / 2;
            break;
        case DIRECTION_SE:
            dr = ROOT3 * Sr / 2;
            dc = 3 * Sc / 2;
            break;
        default:
            break;
    }
    wdraw_line(win, r0, c0, r0 + round(dc), c0 + round(dr), '#');
}


void wdraw_river(WINDOW *win, struct Geometry *g, int r, int c, enum DIRECTION d)
{
    int r0 = 0, c0 = 0, r1 = 0, c1 = 0;
    float Sr = geometry_scale(g) / 2,
          Sc = geometry_scale(g) * geometry_aspect(g) / 2;

    switch (d) {
        case DIRECTION_EE:
            r0 = r + round(ROOT3 * Sr);
            c0 = c + round(Sc);
            r1 = r + round(ROOT3 * Sr);
            c1 = c - round(Sc);
            break;
        case DIRECTION_NE:
            r0 = r + round(ROOT3 * Sr);
            c0 = c - round(Sc);
            r1 = r;
            c1 = c - round(2 * Sc);
            break;
        case DIRECTION_NW:
            r0 = r;
            c0 = c - round(2 * Sc);
            r0 = r - round(ROOT3 * Sr);
            c0 = c - round(Sc);
            break;
        case DIRECTION_WW:
            r0 = r - round(ROOT3 * Sr);
            c0 = c - round(Sc);
            r1 = r - round(ROOT3 * Sr);
            c1 = c + round(Sc);
            break;
        case DIRECTION_SW:
            r0 = r - round(ROOT3 * Sr);
            c0 = c + round(Sc);
            r1 = r;
            c1 = c + round(2 * Sc);
            break;
        case DIRECTION_SE:
            r0 = r;
            c0 = c + round(2 * Sc);
            r1 = r + round(ROOT3 * Sr);
            c1 = c + round(Sc);
            break;
        default:
            break;
    }
    wdraw_line(win, r0, c0, r1, c1, '~');
}


/*
 *      DRAW 02 - Panels
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
        mvwprintw(win, r+2+i, c+2, "%s", line);
    }
}


void wclear_panel(WINDOW *win, struct Panel *p)
{
    int r = panel_row(p), c = panel_col(p);
    int w = panel_width(p), h = panel_height(p);
    wdraw_rectangle(win, r, c, w, h, ' ');
}

/*
 *      DRAW 03 - Chartes and terrain
 */



void wdraw_reticule(WINDOW *win, struct Geometry *g)
{
    char ch = '#';
    int dh = 0;

    float slope = geometry_slope(g);
    int rmid = geometry_rmid(g), cmid = geometry_cmid(g);
    int w_half = (geometry_tile_w(g)+1)/2, h_half = (geometry_tile_h(g))/2;

    attron(COLOR_PAIR(COLOR_RED));

    mvwvline(win, rmid-h_half + 1, cmid-w_half, ch, 2*h_half + 1);
    mvwvline(win, rmid-h_half + 1, cmid+w_half, ch, 2*h_half + 1);

    mvwvline(win, rmid-h_half + 1, cmid-w_half+1, ch, 2*h_half + 1);
    mvwvline(win, rmid-h_half + 1, cmid+w_half-1, ch, 2*h_half + 1);

    for (int col = -w_half; col <= w_half; col++) {
        dh = (col < 0)
            ? floor((w_half+col)*slope)
            : floor((w_half-col)*slope);
        mvwaddch(win, rmid - (h_half + dh), cmid + col, ch);
        mvwaddch(win, rmid + (h_half + dh)+1, cmid + col, ch);
    }

    attroff(COLOR_PAIR(COLOR_RED));
    return;
}


void wdraw_tile(WINDOW *win, struct Geometry *g, struct Tile *tile, int r0, int c0)
{
    int w_half = (geometry_tile_w(g)+1) / 2,
        h_half = (geometry_tile_h(g)+1) / 2;
    int dh = 0;
    char ch = 0;

    enum TERRAIN t = tile_terrain(tile);

    attron(COLOR_PAIR(terrain_colour(t)));
    for (int c = -w_half; c <= w_half; c++) {
        dh = (c < 0)
                ? floor((w_half+c)*geometry_slope(g))
                : floor((w_half-c)*geometry_slope(g));

        for (int r = -(h_half + dh)+1; r <= (h_half + dh); r++) {
            ch = tile_getch(tile, c, r);
            mvwaddch(win, r0 + r, c0 + c, ch);
        }
    }
    attroff(COLOR_PAIR(terrain_colour(t)));

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


void wdraw_chart(
        WINDOW *win,
        struct Geometry *g,
        struct Chart *chart,
        float u0,
        float v0)
{
    /* recurse down the chart to find the tiles at the bottom and print them
     * send the right integer character coordinates */
    if (!chart) {
        return;
    }
    if (chart_children(chart)) {
        for (int i = 0; i < NUM_CHILDREN; i++) {
            wdraw_chart(win, g, chart_child(chart, i), u0, v0);
        }
    }
    if (!chart_tile(chart)) {
        return;
    }
    float u = chart_u(chart), v = chart_v(chart);
    int dc = round(geometry_scale(g) * (u - u0));
    int dr = round(geometry_scale(g) * geometry_aspect(g) * (v - v0));
    wdraw_tile(win, g, chart_tile(chart), geometry_rmid(g) + dr, geometry_cmid(g) + dc);
}


void wdraw_atlas_at(
        WINDOW *win,
        struct Geometry *g,
        struct Atlas *atlas,
        struct Coordinate *c)
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
    struct Chart *centre = atlas_curr(atlas);
    wdraw_chart(win, g, chart, chart_u(centre), chart_v(centre));
}


void wdraw_atlas(WINDOW *win, struct Geometry *g, struct Atlas *atlas)
{
    /* find the smallest coordinate that is guaranteed to contain the window
     * send this coordinate to wdraw_atlas_at
     * */

    struct Chart *centre = atlas_curr(atlas);
    struct Coordinate *c = chart_coordinate(centre);

    /* TODO move this calculation to Geometry struct */
    int n_hor = round( geometry_cols(g) / (1.00f * geometry_tile_w(g))) + 1,
        n_ver = round( geometry_rows(g) / (0.75f * geometry_tile_h(g))) + 1;

    struct Coordinate *c_E = coordinate_duplicate(c),
                      *c_W = coordinate_duplicate(c),
                      *c_N = coordinate_duplicate(c),
                      *c_S = coordinate_duplicate(c);

    coordinate_nshift(c_W, DIRECTION_WW, n_hor/2);
    coordinate_nshift(c_E, DIRECTION_EE, n_hor/2);
    coordinate_nshift(c_N, DIRECTION_NW, n_ver/4);
    coordinate_nshift(c_N, DIRECTION_NE, n_ver/4);
    coordinate_nshift(c_S, DIRECTION_SW, n_ver/4);
    coordinate_nshift(c_S, DIRECTION_SE, n_ver/4);

    struct Coordinate *tmp1 = coordinate_create_ancestor(c_E, c_W);
    struct Coordinate *tmp2 = coordinate_create_ancestor(c_N, c_S);
    struct Coordinate *a    = coordinate_create_ancestor(tmp1, tmp2);

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
 *      DRAW 04 - Core stuff
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
    int r0 = geometry_rows(state_geometry(s))-1,
        c0 = 0,
        w  = geometry_cols(state_geometry(s))-1;

    mvwhline(win, r0, c0, ' ', w);
    attron(COLOR_PAIR(mode_colour(state_mode(s))));
    mvwaddstr(win, r0, c0+1, mode_name(state_mode(s)));
    attroff(COLOR_PAIR(mode_colour(state_mode(s))));

    switch (state_mode(s)) {
        case MODE_COMMAND:
            addch(' ');
            addch(':');
            addstr(commandline_str(state_commandline(s)));
            break;
        case MODE_TERRAIN:
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
