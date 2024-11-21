#include <stdlib.h>
#include <ncurses.h>
#include <math.h>

#include "include/draw.h"
#include "include/geometry.h"
#include "include/grid.h"
#include "include/interface.h"
#include "include/panel.h"
#include "include/state.h"
#include "include/terrain.h"


/*
 *      DRAW 01 - Basic shapes
 */

int draw_border(int r0, int c0, int w, int h)
{
    mvhline(r0,     c0,     ACS_HLINE, w-1);
    mvvline(r0,     c0,     ACS_VLINE, h-1);
    mvvline(r0,     c0+w-1, ACS_VLINE, h-1);
    mvhline(r0+h-1, c0,     ACS_HLINE, w-1);

    mvaddch(r0,     c0,     ACS_PLUS);
    mvaddch(r0,     c0+w-1, ACS_PLUS);
    mvaddch(r0+h-1, c0,     ACS_PLUS);
    mvaddch(r0+h-1, c0+w-1, ACS_PLUS);

    return 0;
}


int draw_rectangle(int r0, int c0, int w, int h, char bg)
{
    for (int r=0; r<h; r++) {
        mvhline(r0+r, c0, bg, w);
    }

    return 0;
}


int draw_box(int r0, int c0, int w, int h, char bg)
{
    draw_rectangle(r0, c0, w, h, bg);
    draw_border(r0, c0, w, h);

    return 0;
}


/*
 *      DRAW 02 - Panels
 */

int draw_panel(struct Panel *p)
{
    int r = panel_row(p), c = panel_col(p);
    int w = panel_width(p), h = panel_height(p);
    draw_box(r, c, w, h, ' ');

    for (int i=0; i<panel_len(p); i++) {
        char *line = panel_line(p, i);
        if (!line) { continue; }
        mvprintw(r+2+i, c+2, "%s", line);
    }
    return 0;
}


int clear_panel(struct Panel *p)
{
    int r = panel_row(p), c = panel_col(p);
    int w = panel_width(p), h = panel_height(p);
    draw_rectangle(r, c, w, h, ' ');

    return 0;
}

/*
 *      DRAW 03 - Hexes and terrain
 */



void draw_reticule(struct Geometry *g)
{
    char ch = '#';
    int dh = 0;

    float slope = geometry_slope(g);
    int rmid = geometry_rmid(g), cmid = geometry_cmid(g);
    int w_half = (geometry_hex_w(g)+1)/2, h_half = (geometry_hex_h(g))/2;

    attron(COLOR_PAIR(COLOR_RED));

    mvvline(rmid-h_half + 1, cmid-w_half, ch, 2*h_half + 1);
    mvvline(rmid-h_half + 1, cmid+w_half, ch, 2*h_half + 1);

    mvvline(rmid-h_half + 1, cmid-w_half+1, ch, 2*h_half + 1);
    mvvline(rmid-h_half + 1, cmid+w_half-1, ch, 2*h_half + 1);

    for (int col = -w_half; col <= w_half; col++) {
        dh = (col < 0)
            ? floor((w_half+col)*slope)
            : floor((w_half-col)*slope);
        mvaddch(rmid - (h_half + dh), cmid + col, ch);
        mvaddch(rmid + (h_half + dh)+1, cmid + col, ch);
    }

    attroff(COLOR_PAIR(COLOR_RED));
    return;
}


int draw_hex(struct Geometry *g, struct Hex *hex, int r0, int c0)
{
    int w_half = (geometry_hex_w(g)+1) / 2,
        h_half = (geometry_hex_h(g)+1) / 2;
    int dh = 0;
    char ch = 0;

    enum TERRAIN t = hex_terrain(hex);
    int s = hex_seed(hex);

    attron(COLOR_PAIR(terrain_colour(t)));
    for (int c = -w_half; c <= w_half; c++) {
        dh = (c < 0)
                ? floor((w_half+c)*geometry_slope(g))
                : floor((w_half-c)*geometry_slope(g));

        for (int r = -(h_half + dh)+1; r <= (h_half + dh); r++) {
            ch = terrain_getch(t, c, r, s);
            mvaddch(r0 + r, c0 + c, ch);
        }
    }
    attroff(COLOR_PAIR(terrain_colour(t)));
    return 0;
}


void draw_map(struct Geometry *g, struct Map *map)
{
    struct Hex *centre = map_curr(map);

    /* calculate the number of hexes that fit to screen */
    int n_hor = round( geometry_cols(g) / (1.00f * geometry_hex_w(g))) + 1,
        n_ver = round( geometry_rows(g) / (0.75f * geometry_hex_h(g))) + 1;

    /* set up variables for checking the geometry of each hex */
    float u0 = hex_u(centre),
          v0 = hex_v(centre);
    float u = 0, v = 0;
    int dr = 0, dc = 0;

    struct Coordinate *edge = coordinate_duplicate(hex_coordinate(centre)),
                      *target = coordinate_duplicate(coordinate_zero());
    struct Hex *hex = NULL;

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
            hex = map_find(map, target);
            if (hex) {
                u = hex_u(hex);
                v = hex_v(hex);
                dc = round(geometry_scale(g) * (u - u0));
                dr = round(geometry_scale(g) * geometry_aspect(g) * (v - v0));
                draw_hex(g, hex, geometry_rmid(g) + dr, geometry_cmid(g) + dc);
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


void draw_ui(struct UserInterface *ui)
{
    for (int p=0; p<NUM_UI_PANELS; p++) {
        if (ui_show(ui, p)) {
            draw_panel(ui_panel(ui, p));
        }
    }
    return;
}


void draw_statusline(struct StateManager *sm)
{
    int r0 = geometry_rows(state_geometry(sm))-1,
        c0 = 0,
        w  = geometry_cols(state_geometry(sm))-1;

    mvhline(r0, c0, ' ', w);
    attron(COLOR_PAIR(state_mode_colour(sm)));
    mvaddstr(r0, c0+1, state_mode_name(sm));
    attroff(COLOR_PAIR(state_mode_colour(sm)));
    if (state_mode(sm) == INPUT_COMMAND) {
        addch(' ');
        addch(':');
        addstr(state_charbuf(sm));
    }
    return;
}


void draw_state(struct StateManager *sm)
{
    struct Geometry *g = state_geometry(sm);

    draw_map(g, state_map(sm));
    draw_reticule(g);
    draw_ui(state_ui(sm));
    draw_statusline(sm);
    return;
}
