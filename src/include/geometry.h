#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <ncurses.h>

#define GEOMETRY_DEFAULT_ASPECT 0.67f
#define GEOMETRY_DEFAULT_SCALE  10

#define ROOT3       1.732050807f
#define ROOT3_INV   0.57735026919f

void geometry_initialise(float scale, float aspect, WINDOW * win);
float geometry_slope(void);
int geometry_cmid(void);
int geometry_rmid(void);
int geometry_rows(void);
int geometry_cols(void);
int geometry_tile_dh(void);
int geometry_tile_dw(void);
int geometry_tile_nh(void);
int geometry_tile_nw(void);

#endif
