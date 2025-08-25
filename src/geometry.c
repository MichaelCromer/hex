#include <stdlib.h>
#include <math.h>

#include "hdr/geometry.h"

float scale;
float aspect;
float slope;
int tile_dw, tile_dh;
int tile_nw, tile_nh;
int cols, rows;
int rmid, cmid;

struct Coordinate screen_L = { 0 };
struct Coordinate screen_R = { 0 };
struct Coordinate screen_T = { 0 };
struct Coordinate screen_B = { 0 };
struct Coordinate viewpoint = { 0 };


void geometry_rescale(float scale_new)
{
    scale = scale_new;
    tile_dw = round(scale * ROOT3 / 2);
    tile_dh = round(scale * aspect / 2);
    tile_nw = round(cols/(2.00f*tile_dw)) + 1;
    tile_nh = round(rows/(1.50f*tile_dh)) + 1;

    geometry_calculate_viewpoint(coordinate_origin());
}


void geometry_initialise(WINDOW *win)
{
    aspect = GEOMETRY_DEFAULT_ASPECT;
    scale = GEOMETRY_DEFAULT_SCALE;
    getmaxyx(win, rows, cols);

    slope = ROOT3_INV * aspect;
    rmid = rows / 2;
    cmid = cols / 2;

    tile_dw = round(scale * ROOT3 / 2);
    tile_dh = round(scale * aspect / 2);
    tile_nw = round(cols/(2.00f*tile_dw)) + 1;
    tile_nh = round(rows/(1.50f*tile_dh)) + 1;

    geometry_calculate_viewpoint(coordinate_origin());
}


void geometry_calculate_viewpoint(struct Coordinate o)
{
    screen_L = coordinate_nshift(o, DIRECTION_WW, tile_nw / 2);
    screen_R = coordinate_nshift(o, DIRECTION_EE, tile_nw / 2);
    screen_T = coordinate_nshift(o, DIRECTION_NW, tile_nh / 4);
    screen_B = coordinate_nshift(o, DIRECTION_SW, tile_nh / 4);
    screen_T = coordinate_nshift(screen_T, DIRECTION_NE, tile_nh / 4);
    screen_B = coordinate_nshift(screen_B, DIRECTION_SE, tile_nh / 4);

    viewpoint = coordinate_common_ancestor(
        coordinate_common_ancestor(screen_L, screen_R),
        coordinate_common_ancestor(screen_T, screen_B)
    );
}


void geometry_zoom(bool out)
{
    if (out) {
        if (scale <= 3) return;
        geometry_rescale(scale - 1);
    } else {
        if (scale >= 24) return;
        geometry_rescale(scale + 1);
    }
}


float geometry_slope(void) { return slope; }
int geometry_tile_dh(void) { return tile_dh; }
int geometry_tile_dw(void) { return tile_dw; }
int geometry_tile_nh(void) { return tile_nh; }
int geometry_tile_nw(void) { return tile_nw; }
int geometry_cols(void) { return cols; }
int geometry_rows(void) { return rows; }
int geometry_rmid(void) { return rmid; }
int geometry_cmid(void) { return cmid; }
struct Coordinate geometry_viewpoint(void) { return viewpoint; }
