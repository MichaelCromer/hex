#ifndef DRAW_H
#define DRAW_H

#include "panel.h"
#include "grid.h"
#include "interface.h"
#include "geometry.h"

int draw_panel(struct Panel *p);
int draw_box(int r0, int c0, int w, int h, char bg);
void draw_ui(struct UserInterface *ui);

void draw_screen(struct Geometry *g, struct Map *map, struct UserInterface *ui);

#endif
