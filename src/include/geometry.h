#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <ncurses.h>

#define GEOMETRY_DEFAULT_ASPECT 0.66f
#define GEOMETRY_DEFAULT_SCALE  10

#define ROOT3       1.732050807f
#define ROOT3_INV   0.57735026919f


struct Geometry;

struct Geometry *geometry_create(float scale, float aspect, WINDOW *win);
void geometry_destroy(struct Geometry *g);
void geometry_set_scale(struct Geometry *g, float scale);
void geometry_set_aspect(struct Geometry *g, float aspect);
int geometry_cmid(struct Geometry *g);
int geometry_rmid(struct Geometry *g);
int geometry_rows(struct Geometry *g);
int geometry_cols(struct Geometry *g);
float geometry_scale(struct Geometry *g);
float geometry_slope(struct Geometry *g);
float geometry_aspect(struct Geometry *g);
int geometry_hex_h(struct Geometry *g);
int geometry_hex_w(struct Geometry *g);


#endif