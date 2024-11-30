#include <stdlib.h>
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


int wdraw_hex(WINDOW *win, struct Geometry *g, struct Chart *chart, int r0, int c0)
{
    int w_half = (geometry_tile_w(g)+1) / 2,
        h_half = (geometry_tile_h(g)+1) / 2;
    int dh = 0;
    char ch = 0;

    enum TERRAIN t = chart_terrain(chart);

    attron(COLOR_PAIR(terrain_colour(t)));
    for (int c = -w_half; c <= w_half; c++) {
        dh = (c < 0)
                ? floor((w_half+c)*geometry_slope(g))
                : floor((w_half-c)*geometry_slope(g));

        for (int r = -(h_half + dh)+1; r <= (h_half + dh); r++) {
            ch = tile_getch(chart_tile(chart), c, r);
            mvwaddch(win, r0 + r, c0 + c, ch);
        }
    }
    attroff(COLOR_PAIR(terrain_colour(t)));
    return 0;
}


void wdraw_atlas(WINDOW *win, struct Geometry *g, struct Atlas *atlas)
{
    struct Chart *centre = atlas_curr(atlas);

    /* calculate the number of hexes that fit to screen */
    int n_hor = round( geometry_cols(g) / (1.00f * geometry_tile_w(g))) + 1,
        n_ver = round( geometry_rows(g) / (0.75f * geometry_tile_h(g))) + 1;

    /* set up variables for checking the geometry of each chart */
    float u0 = chart_u(centre),
          v0 = chart_v(centre);
    float u = 0, v = 0;
    int dr = 0, dc = 0;

    struct Coordinate *edge = coordinate_duplicate(chart_coordinate(centre)),
                      *target = coordinate_duplicate(coordinate_origin());
    struct Chart *chart = NULL;

    for (int y=0; y<(n_ver/2); y++) {
        /* pair results in (-1, 2, -1) */
        coordinate_shift(edge, (y % 2) ? SOUTHWEST : SOUTHEAST);
    }
    for (int x=0; x<(n_hor/2); x++) {
        /* single results in (-1, 0, 1) */
        coordinate_shift(edge, WEST);
    }

    for (int y=0; y<n_ver; y++) {
        coordinate_copy(edge, target);
        for (int x=0; x<n_hor; x++) {
            chart = atlas_find(atlas, target);
            if (chart) {
                u = chart_u(chart);
                v = chart_v(chart);
                dc = round(geometry_scale(g) * (u - u0));
                dr = round(geometry_scale(g) * geometry_aspect(g) * (v - v0));
                wdraw_hex(win, g, chart, geometry_rmid(g) + dr, geometry_cmid(g) + dc);
            }
            coordinate_shift(target, EAST);
        }
        coordinate_shift(edge, (y % 2) ? NORTHEAST : NORTHWEST);
    }

    coordinate_destroy(edge);
    coordinate_destroy(target);
    return;
}

/*
 *      DRAW 04 - Core stuff
 */


void wdraw_ui(WINDOW *win, struct UserInterface *ui)
{
    for (int p=0; p<NUM_UI_PANELS; p++) {
        if (ui_show(ui, p)) {
            wdraw_panel(win, ui_panel(ui, p));
        }
    }
    return;
}


void wdraw_statusline(WINDOW *win, struct State *s)
{
    int r0 = geometry_rows(state_geometry(s))-1,
        c0 = 0,
        w  = geometry_cols(state_geometry(s))-1;

    mvwhline(win, r0, c0, ' ', w);
    attron(COLOR_PAIR(state_mode_colour(s)));
    mvwaddstr(win, r0, c0+1, state_mode_name(s));
    attroff(COLOR_PAIR(state_mode_colour(s)));

    switch (state_mode(s)) {
        case INPUT_MODE_COMMAND:
            addch(' ');
            addch(':');
            addstr(commandline_str(state_commandline(s)));
            break;
        case INPUT_MODE_TERRAIN:
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

    return;
}


void draw_state(struct State *s)
{
    wdraw_atlas(state_window(s), state_geometry(s), state_atlas(s));
    wdraw_reticule(state_window(s), state_geometry(s));
    wdraw_ui(state_window(s), state_ui(s));
    wdraw_statusline(state_window(s), s);
    return;
}
