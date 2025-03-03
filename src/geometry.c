#include <stdlib.h>
#include <math.h>

#include "include/geometry.h"

float scale;
float aspect;
float slope;
int tile_dw, tile_dh;
int tile_nw, tile_nh;
int cols, rows;
int rmid, cmid;

void geometry_initialise(float scale, float aspect, WINDOW *win)
{
    scale = scale;
    aspect = aspect;
    getmaxyx(win, rows, cols);

    tile_dw = round(scale * ROOT3 / 2);
    tile_dh = round(scale * aspect / 2);
    tile_nw = round(cols/(2.00f*tile_dw)) + 1;
    tile_nh = round(rows/(1.50f*tile_dh)) + 1;

    slope = ROOT3_INV * aspect;
    rmid = rows / 2;
    cmid = cols / 2;
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
