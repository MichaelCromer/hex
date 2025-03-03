#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <ncurses.h>

#define GEOMETRY_DEFAULT_ASPECT 0.67f
#define GEOMETRY_DEFAULT_SCALE  10

#define ROOT3       1.732050807f
#define ROOT3_INV   0.57735026919f

struct Geometry;

struct Geometry *geometry_create(void);
void geometry_initialise(struct Geometry *g, float scale, float aspect, WINDOW * win);
void geometry_destroy(struct Geometry *g);
float geometry_slope(struct Geometry *g);
int geometry_cmid(struct Geometry *g);
int geometry_rmid(struct Geometry *g);
int geometry_rows(struct Geometry *g);
int geometry_cols(struct Geometry *g);
int geometry_tile_dh(struct Geometry *g);
int geometry_tile_dw(struct Geometry *g);
int geometry_tile_nh(struct Geometry *g);
int geometry_tile_nw(struct Geometry *g);
struct Coordinate geometry_viewpoint(struct Geometry *g);
void geometry_step(struct Geometry *g, enum DIRECTION d);

#endif
