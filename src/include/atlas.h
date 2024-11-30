#ifndef GRID_H
#define GRID_H

#include "coordinate.h"
#include "geometry.h"
#include "enum.h"

struct Chart;
struct Chart *chart_create(const struct Coordinate *c);
struct Chart *chart_create_ancestor(struct Chart *hex1, struct Chart *hex2);
struct Chart *chart_create_origin(void);
struct Chart *chart_child(const struct Chart *chart, enum CHILDREN c);
void chart_set_child(struct Chart *chart, enum CHILDREN c, struct Chart *child);
void chart_destroy(struct Chart *chart);
struct Coordinate *chart_coordinate(const struct Chart *chart);
struct Tile *chart_tile(const struct Chart *chart);
void chart_set_tile(struct Chart *chart, struct Tile *tile);
int chart_seed(const struct Chart *chart);
int chart_p(const struct Chart *chart);
int chart_q(const struct Chart *chart);
int chart_r(const struct Chart *chart);
unsigned int chart_m(const struct Chart *chart);
float chart_u(const struct Chart *chart);
float chart_v(const struct Chart *chart);
enum TERRAIN chart_terrain(const struct Chart *chart);
void chart_set_terrain(struct Chart *chart, enum TERRAIN t);

struct Atlas;
struct Atlas *atlas_create(void);
void atlas_initialise(struct Atlas *m, struct Chart *root);
void atlas_destroy(struct Atlas *m);
struct Chart *atlas_root(const struct Atlas *m);
struct Chart *atlas_curr(const struct Atlas *m);
struct Coordinate *atlas_coordinate(const struct Atlas *atlas);
struct Tile *atlas_tile(const struct Atlas *atlas);
enum TERRAIN atlas_terrain(const struct Atlas *m);
void atlas_set_terrain(struct Atlas *m, enum TERRAIN t);
void atlas_goto(struct Atlas *atlas, const struct Coordinate *c);
struct Chart *atlas_neighbour(const struct Atlas *m, enum DIRECTION d);
void atlas_step(struct Atlas *m, enum DIRECTION d);
struct Chart *atlas_find(const struct Atlas *atlas, const struct Coordinate *c);
void atlas_insert(struct Atlas *atlas, struct Chart *new);
void atlas_create_neighbours(struct Atlas *atlas);

#endif
