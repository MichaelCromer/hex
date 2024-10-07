#ifndef DRAW_H
#define DRAW_H

#include "grid.h"
#include "panel.h"

int draw_border(int r0, int c0, int w, int h);
int draw_rectangle(int r0, int c0, int w, int h, char bg);
int draw_box(int r0, int c0, int w, int h, char bg);
int draw_panel(struct Panel *p);
int clear_panel(struct Panel *p);
char get_terrainchr(enum TERRAIN t);
void draw_reticule(int row0, int col0, int w_half, int h_half, float slope);
int draw_hex(struct Hex *hex, int row0, int col0, int w_half, int h_half, float slope);
void draw_map(struct Hex *origin, float scale, float aspect_rato, int rows, int cols);
int draw_screen(float scale, float aspect_rato, int cols, int rows, struct Hex *map);

#endif
