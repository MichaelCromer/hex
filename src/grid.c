#include <math.h>
#include <stdlib.h>
#include <stdbool.h>

#include "grid.h"


/*
 * Terrain lookups
 *
 */

const char *terrain_none = "None";
const char *terrain_water = "Water";
const char *terrain_mountains = "Mountains";
const char *terrain_plains = "Plains";
const char *terrain_hills = "Hills";
const char *terrain_forest = "Forset";
const char *terrain_desert = "Desert";
const char *terrain_jungle = "Jungle";
const char *terrain_swamp = "Swamp";


const char *terrain_string(enum TERRAIN t)
{
    switch (t) {
        case WATER:
            return terrain_water;
        case MOUNTAINS:
            return terrain_mountains;
        case PLAINS:
            return terrain_plains;
        case HILLS:
            return terrain_hills;
        case FOREST:
            return terrain_forest;
        case DESERT:
            return terrain_desert;
        case JUNGLE:
            return terrain_jungle;
        case SWAMP:
            return terrain_swamp;
        default:
            break;
    }
    return terrain_none;
}

struct Hex {
    struct Coordinate *coordinate;
    enum TERRAIN terrain;
    struct Hex **children;
};


struct Map {
    struct Hex *root;
    struct Hex *curr;
};


/*
 *      HEX Functions
 */


struct Hex *hex_create(const struct Coordinate *c)
{
    /* try allocate hex */
    struct Hex *h = malloc(sizeof(struct Hex));
    if (h == NULL) {
        return NULL;
    }

    /* try allocate coordinate */
    h->coordinate = coordinate_create(0,0,0,0);
    if (h->coordinate == NULL) {
        free(h);
        h = NULL;
        return NULL;
    }
    coordinate_copy(c, h->coordinate);

    /* try allocate children */
    if (coordinate_magnitude(c) > 0) {
        h->children = malloc(9 * sizeof(struct Hex *));
        if (h->children == NULL) {
            coordinate_destroy(h->coordinate);
            free(h);
            h = NULL;
            return NULL;
        }
        for (int i=0; i<9; i++) {
            h->children[i] = NULL;
        }
    } else {
        h->children = NULL;
    }

    /* other data */
    /* TODO? separate struct Tile containing all terrain-like data, for nullability */
    h->terrain = NONE;

    return h;
}


struct Hex *hex_origin(void)
{
    const struct Coordinate *zero = coordinate_zero();
    return hex_create(zero);
}


struct Hex *hex_child(const struct Hex *parent, int i)
{
    if (parent->children == NULL) {
        return NULL;
    }

    if ( i < 0 || i > 8 ) {
        return NULL;
    }

    return parent->children[i];
}


void hex_set_child(struct Hex *parent, int i, struct Hex *child)
{
    if (hex_child(parent, i)) {
        return;
    }

    parent->children[i] = child;
}


void hex_destroy(struct Hex *h)
{
    if (h == NULL) {
        return;
    }

    if (h->children != NULL) {
        for (int i=0; i<9; i++) {
            hex_destroy(hex_child(h, i));
        }
        free(h->children);
        h->children = NULL;
    }

    coordinate_destroy(h->coordinate);

    free(h);
    h = NULL;

    return;
}


struct Coordinate *hex_coordinate(struct Hex *hex)
{
    return hex->coordinate;
}


unsigned int hex_m(struct Hex *hex)
{
    return coordinate_magnitude(hex->coordinate);
}


int hex_p(struct Hex *hex)
{
    return coordinate_p(hex->coordinate);
}


int hex_q(struct Hex *hex)
{
    return coordinate_q(hex->coordinate);
}


int hex_r(struct Hex *hex)
{
    return coordinate_r(hex->coordinate);
}


float hex_u(struct Hex *hex)
{
    return ROOT3 * (hex_p(hex) + hex_q(hex)/2.0f);
}


float hex_v(struct Hex *hex)
{
    return 1.5f * hex_q(hex);
}


void hex_set_terrain(struct Hex *h, enum TERRAIN t)
{
    h->terrain = t;
    return;
}


enum TERRAIN hex_terrain(struct Hex *h)
{
    return h->terrain;
}


struct Hex *hex_find(struct Hex *root, struct Coordinate *target)
{
    /* pathological -- null root or unrelated coordinates */
    if ((root == NULL) || !coordinate_related(root->coordinate, target)) {
        return NULL;
    }

    /* we found it */
    if (coordinate_equals(root->coordinate, target)) {
        return root;
    }

    /* now continue to children */
    if (root->children == NULL) {
        return NULL;
    }

    /* find the right child block to check in */
    struct Coordinate *zoom = coordinate_duplicate(target);
    coordinate_lift_to(zoom, coordinate_magnitude(root->coordinate) - 1);
    int child_index = coordinate_index(zoom);
    coordinate_destroy(zoom);

    /* recurse to find within zoomed block */
    return hex_find(
            hex_child(root, child_index),
            target
        );
}


void hex_insert(struct Hex **root, struct Hex *hex)
{
    if (root == NULL || *root == NULL || hex == NULL ) {
        return;
    }

    struct Coordinate *origin = (*root)->coordinate;
    struct Coordinate *target = hex->coordinate;

    if (!coordinate_related(origin, target)) {
        struct Coordinate *ancestor = coordinate_duplicate(coordinate_zero());
        coordinate_common_ancestor(origin, target, ancestor);
        struct Hex *top = hex_create(ancestor);
        hex_insert(&top, *root);
        hex_insert(&top, hex);
        *root = top;
        coordinate_destroy(ancestor);
        return;
    }

    if (hex_find(*root, target)) {
        return;
    }

    struct Coordinate *above = coordinate_duplicate(coordinate_zero());
    coordinate_parent(target, above);
    struct Hex *parent = hex_find(*root, above);

    if (!parent) {
        parent = hex_create(above);
        hex_insert(root, parent);
    }

    coordinate_destroy(above);
    hex_set_child(parent, coordinate_index(target), hex);

    return;
}


struct Hex *hex_neighbour(struct Hex *root, struct Hex *hex, enum DIRECTION d)
{
    struct Coordinate *target = coordinate_duplicate(coordinate_zero());
    coordinate_add(hex->coordinate, coordinate_delta(d), target);
    struct Hex *found = hex_find(root, target);
    coordinate_destroy(target);

    return found;
}


void hex_create_neighbours(struct Hex **root, struct Hex *hex)
{
    struct Coordinate *location = coordinate_duplicate(coordinate_zero());
    static const struct Coordinate *d_loc;
    struct Hex *neighbour = NULL;
    for (int i=0; i<6; i++) {
        d_loc = coordinate_delta(i);
        coordinate_add(hex->coordinate, d_loc, location);
        neighbour = hex_find(*root, location);
        if (!neighbour) {
            neighbour = hex_create(location);
            hex_insert(root, neighbour);
        }
    }

    coordinate_destroy(location);
    return;
}

