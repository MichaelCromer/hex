#include <stdlib.h>
#include <math.h>

#include "include/geometry.h"


struct Geometry {
    float scale;
    float aspect;
    float slope;
    int chart_w, chart_h;
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

    g->chart_w = round(g->scale * ROOT3);
    g->chart_h = round(g->scale * g->aspect);
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

float geometry_scale(struct Geometry *g)
{
    return g->scale;
}

float geometry_aspect(struct Geometry *g)
{
    return g->aspect;
}

float geometry_slope(struct Geometry *g)
{
    return g->slope;
}

int geometry_tile_h(struct Geometry *g)
{
    return g->chart_h;
}

int geometry_tile_w(struct Geometry *g)
{
    return g->chart_w;
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


/* Geometry setters */

void geometry_set_scale(struct Geometry *g, float scale)
{
    g->scale = scale;
    g->chart_w = round(g->scale * ROOT3);
    g->chart_h = round(g->scale * g->aspect);
}


void geometry_set_aspect(struct Geometry *g, float aspect)
{
    g->aspect = aspect;
    g->chart_h = round(g->scale * g->aspect);
    g->slope = ROOT3_INV * g->aspect;
}


