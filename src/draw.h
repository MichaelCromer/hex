#ifndef DRAW_H
#define DRAW_H

#include "grid.h"
#include "panel.h"

struct Geometry;

struct Geometry *geometry_create(float scale, float aspect, int cols, int rows);
void geometry_set_scale(struct Geometry *g, float scale);
void geometry_set_aspect(struct Geometry *g, float aspect);
int geometry_cmid(struct Geometry *g);
int geometry_rmid(struct Geometry *g);
int draw_panel(struct Panel *p);
int draw_screen(struct Geometry *g, struct Hex *map, struct Hex *centre);

#endif
