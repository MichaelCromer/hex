#ifndef GRID_H
#define GRID_H


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

/* Basics */
struct Hex *hex_create(void);
void hex_destroy(struct Hex *h);

/* Terrain-like */
void hex_set_terrain(struct Hex *h, enum TERRAIN t);
enum TERRAIN hex_get_terrain(struct Hex *h);

/* Geometry-like */
void hex_set_coords(struct Hex *h, int x, int y, int z);
int hex_x(struct Hex *h);
int hex_y(struct Hex *h);
int hex_z(struct Hex *h);

#endif
