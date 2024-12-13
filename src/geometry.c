#include <stdlib.h>
#include <math.h>

#include "include/geometry.h"

struct Geometry {
    float scale;
    float aspect;
    float slope;
    int tile_dw, tile_dh;
    int tile_nw, tile_nh;
    int cols, rows;
    int rmid, cmid;
};

struct Geometry *geometry_create(void)
{
    struct Geometry *g = malloc(sizeof(struct Geometry));
    return g;
}

void geometry_initialise(struct Geometry *g, float scale, float aspect, WINDOW *win)
{
    g->scale = scale;
    g->aspect = aspect;
    getmaxyx(win, g->rows, g->cols);

    g->tile_dw = round(g->scale * ROOT3 / 2);
    g->tile_dh = round(g->scale * g->aspect / 2);
    g->tile_nw = round(g->cols/(2.00f*g->tile_dw)) + 1;
    g->tile_nh = round(g->rows/(1.50f*g->tile_dh)) + 1;

    g->slope = ROOT3_INV * g->aspect;
    g->rmid = g->rows / 2;
    g->cmid = g->cols / 2;
}

void geometry_destroy(struct Geometry *g)
{
    free(g);
    g = NULL;
}

float geometry_slope(struct Geometry *g)
{
    return g->slope;
}

int geometry_tile_dh(struct Geometry *g)
{
    return g->tile_dh;
}

int geometry_tile_dw(struct Geometry *g)
{
    return g->tile_dw;
}

int geometry_tile_nh(struct Geometry *g)
{
    return g->tile_nh;
}

int geometry_tile_nw(struct Geometry *g)
{
    return g->tile_nw;
}

int geometry_cols(struct Geometry *g)
{
    return g->cols;
}

int geometry_rows(struct Geometry *g)
{
    return g->rows;
}

int geometry_rmid(struct Geometry *g)
{
    return g->rmid;
}

int geometry_cmid(struct Geometry *g)
{
    return g->cmid;
}
