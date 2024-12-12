#ifndef GRID_H
#define GRID_H

#include "coordinate.h"
#include "geometry.h"
#include "enum.h"

struct Chart;
struct Chart **chart_children(const struct Chart *chart);
struct Chart *chart_child(const struct Chart *chart, enum CHILDREN c);
struct Coordinate *chart_coordinate(const struct Chart *chart);
struct Tile *chart_tile(const struct Chart *chart);
int chart_p(const struct Chart *chart);
int chart_q(const struct Chart *chart);
int chart_r(const struct Chart *chart);

struct Atlas;
struct Atlas *atlas_create(void);
void atlas_initialise(struct Atlas *atlas, struct Geometry *g);
void atlas_destroy(struct Atlas *atlas);
struct Directory *atlas_directory(const struct Atlas *atlas);
struct Chart *atlas_root(const struct Atlas *atlas);
struct Chart *atlas_curr(const struct Atlas *atlas);
struct Coordinate *atlas_coordinate(const struct Atlas *atlas);
struct Tile *atlas_tile(const struct Atlas *atlas);
enum TERRAIN atlas_terrain(const struct Atlas *atlas);
void atlas_set_terrain(struct Atlas *atlas, enum TERRAIN t);
struct Chart *atlas_neighbour(const struct Atlas *atlas, enum DIRECTION d);
void atlas_step(struct Atlas *atlas, enum DIRECTION d);
struct Chart *atlas_find(const struct Atlas *atlas, const struct Coordinate *c);
void atlas_create_neighbours(struct Atlas *atlas);
void atlas_create_location(struct Atlas *atlas, enum LOCATION t);
struct Coordinate *atlas_viewpoint(struct Atlas *atlas);
void atlas_recalculate_viewpoint(struct Atlas *atlas);
void atlas_recalculate_screen(struct Atlas *atlas, struct Geometry *g);

#endif
