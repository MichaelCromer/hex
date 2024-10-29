#ifndef GRID_H
#define GRID_H

#define ROOT3       1.732050807f
#define ROOT3_INV   0.57735026919f

#include "coordinate.h"

enum TERRAIN {
    NONE,
    WATER,
    MOUNTAINS,
    PLAINS,
    HILLS,
    FOREST,
    DESERT,
    JUNGLE,
    SWAMP
};


/***************************************************
 *  TERRAIN
 ***************************************************/

const char *terrain_string(enum TERRAIN t);

/***************************************************
 *  HEX
 ***************************************************/
struct Hex;

/* Basics */
struct Hex *hex_create(const struct Coordinate *c);
struct Hex *hex_origin(void);
void hex_destroy(struct Hex *h);

/* Terrain-like */
void hex_set_terrain(struct Hex *h, enum TERRAIN t);
enum TERRAIN hex_terrain(struct Hex *h);

/* Geometry-like */
int hex_p(struct Hex *hex);
int hex_q(struct Hex *hex);
int hex_r(struct Hex *hex);
float hex_u(struct Hex *h);
float hex_v(struct Hex *h);
struct Coordinate *hex_coordinate(struct Hex *hex);
struct Hex *hex_neighbour(struct Hex *root, struct Hex *hex, enum DIRECTION d);

/* Filling hexes */
struct Hex *hex_find(struct Hex *root, struct Coordinate *target);
void hex_insert(struct Hex **root, struct Hex *hex);
void hex_create_neighbours(struct Hex **root, struct Hex *hex);

#endif