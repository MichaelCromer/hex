#ifndef GRID_H
#define GRID_H

#define ROOT3       1.732050807f
#define ROOT3_INV   0.57735026919f

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


enum DIRECTION {
    EAST,
    NORTHEAST,
    NORTHWEST,
    WEST,
    SOUTHWEST,
    SOUTHEAST
};


/***************************************************
 *  HEX
 ***************************************************/
struct Hex;
struct Coordinate;

/* Basics */
struct Hex *hex_create(void);
void hex_destroy(struct Hex *h);

/* Terrain-like */
void hex_set_terrain(struct Hex *h, enum TERRAIN t);
enum TERRAIN hex_terrain(struct Hex *h);

/* Geometry-like */
float hex_u(struct Hex *h);
float hex_v(struct Hex *h);
struct Hex *hex_neighbour(struct Hex *h, enum DIRECTION d);

/* Filling hexes */
struct Hex *hex_find(struct Hex *start, struct Coordinate *target);
void hex_create_neighbour(struct Hex *h, enum DIRECTION d);
void hex_create_neighbours(struct Hex *h);

#endif
