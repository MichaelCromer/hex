#include <stdlib.h>
#include <math.h>

#include "include/geometry.h"


struct Geometry {
    float scale;
    float aspect;
    float slope;
    int tile_dw, tile_dh;
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
    g->slope = ROOT3_INV * g->aspect;
    g->rmid = g->rows / 2;
    g->cmid = g->cols / 2;

}


void geometry_destroy(struct Geometry *g)
{
    free(g);
    g = NULL;
}


/* Geometry getters */

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
