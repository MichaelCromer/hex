#include <stdlib.h>
#include <math.h>

#include "include/geometry.h"


struct Geometry {
    float scale;
    float aspect;
    float slope;
    int hex_w, hex_h;
    int cols, rows;
    int rmid, cmid;
};


struct Geometry *geometry_create(float scale, float aspect, WINDOW *win)
{
    struct Geometry *g = malloc(sizeof(struct Geometry));

    g->scale = scale;
    g->aspect = aspect;
    getmaxyx(win, g->rows, g->cols);

    g->hex_w = round(g->scale * ROOT3);
    g->hex_h = round(g->scale * g->aspect);
    g->slope = ROOT3_INV * g->aspect;
    g->rmid = g->rows / 2;
    g->cmid = g->cols / 2;


    return g;
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

int geometry_hex_h(struct Geometry *g)
{
    return g->hex_h;
}

int geometry_hex_w(struct Geometry *g)
{
    return g->hex_w;
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
    g->hex_w = round(g->scale * ROOT3);
    g->hex_h = round(g->scale * g->aspect);
}


void geometry_set_aspect(struct Geometry *g, float aspect)
{
    g->aspect = aspect;
    g->hex_h = round(g->scale * g->aspect);
    g->slope = ROOT3_INV * g->aspect;
}


