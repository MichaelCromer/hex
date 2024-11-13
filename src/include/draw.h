#ifndef DRAW_H
#define DRAW_H

#include "panel.h"
#include "grid.h"
#include "interface.h"

struct Geometry;

struct Geometry *geometry_create(float scale, float aspect, int cols, int rows);
void geometry_destroy(struct Geometry *g);
void geometry_set_scale(struct Geometry *g, float scale);
void geometry_set_aspect(struct Geometry *g, float aspect);
int geometry_cmid(struct Geometry *g);
int geometry_rmid(struct Geometry *g);
int geometry_rows(struct Geometry *g);
int geometry_cols(struct Geometry *g);
int draw_panel(struct Panel *p);
int draw_box(int r0, int c0, int w, int h, char bg);
void draw_ui(struct UserInterface *ui);

void draw_screen(struct Geometry *g, struct Map *map, struct UserInterface *ui);

#endif
