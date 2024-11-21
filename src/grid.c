#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "include/grid.h"


static int seed_count = 0; /* TODO change to time? */

int seed_gen(const struct Coordinate *c)
{
    int x = coordinate_p(c),
        y = coordinate_q(c),
        z = coordinate_r(c);
    return (x * 73856093) ^ (y * 19349963) ^ (z * 83492791) ^ (seed_count++ * 5821);
}

int seed_prng(const int x, const int y, const int seed, const int max)
{
    return ((((x * 73856093) ^ (y * 19349963) ^ (seed * 83492791)) % max) + max) % max;
}

struct Hex {
    int seed;
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
    h->seed = seed_gen(c);
    h->terrain = TERRAIN_UNKNOWN;

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


int hex_seed(struct Hex *hex)
{
    return hex->seed;
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
    for (int i = 0; i < NUM_DIRECTIONS; i++) {
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


/* MAP functions */

struct Map *map_create(void)
{
    struct Map *m = malloc(sizeof(struct Map));

    m->root = NULL;
    m->curr = NULL;

    return m;
}


void map_initialise(struct Map *m, struct Hex *root)
{
    if (m->root) {
        return;
    }

    m->root = root;
    m->curr = root;
}


void map_destroy(struct Map *m)
{
    if (!m) {
        return;
    }

    if (m->root) {
        hex_destroy(m->root);
    }

    m->curr = NULL;
    m->root = NULL;

    free(m);
    m = NULL;

    return;
}


struct Coordinate *map_coordinate(const struct Map *m)
{
    return m->curr->coordinate;
}


enum TERRAIN map_curr_terrain(const struct Map *m)
{
    return m->curr->terrain;
}


struct Hex *map_curr(const struct Map *m)
{
    return m->curr;
}


struct Hex *map_find(const struct Map *m, const struct Coordinate *c)
{
    if (!m || !c) {
        return NULL;
    }

    int dm = coordinate_magnitude(m->root->coordinate) - coordinate_magnitude(c);
    if (dm <= 0) {
        if (coordinate_equals(m->root->coordinate, c)) {
            return m->root;
        }
        return NULL;
    }

    int path[dm];
    memset(path, 0, dm * sizeof(int));

    struct Coordinate *zoom = coordinate_duplicate(c);
    struct Hex *target = m->root;

    /* load up the path of patches to zoom into */
    for (int i = 1; i <= dm; i++) {
        path[dm - i] = coordinate_index(zoom);
        coordinate_lift_by(zoom, 1);
    }

    if (!coordinate_equals(m->root->coordinate, zoom)) {
        /* we backtracked the path but target is not under root */
        return NULL;
    }

    /* execute the path */
    for (int i = 0; i < dm; i++) {
        target = hex_child(target, path[i]);
        if (!target) {
            break;
        }
    }

    coordinate_destroy(zoom);
    return target;
}


void map_goto(struct Map *m, const struct Coordinate *c)
{
    struct Hex *new = map_find(m, c);
    if (new) {
        m->curr = new;
    }
    return;
}


/* TODO a map-native implementation of map_insert */
void map_insert(struct Map *m, struct Hex *h)
{
    hex_insert(&(m->root), h);
    return;
}


void map_step(struct Map *m, enum DIRECTION d)
{
    struct Coordinate *target = coordinate_duplicate(m->curr->coordinate);
    coordinate_shift(target, d);
    map_goto(m, target);
    coordinate_destroy(target);
    return;
}


struct Hex *map_neighbour(struct Map *m, enum DIRECTION d)
{
    struct Coordinate *target = coordinate_duplicate(m->curr->coordinate);
    struct Hex *result = NULL;
    coordinate_shift(target, d);
    result = map_find(m, target);
    coordinate_destroy(target);
    return result;
}


/* TODO a map-natrve implementation of map_create_neighbours */
void map_create_neighbours(struct Map *m)
{
    hex_create_neighbours(&(m->root), m->curr);
    return;
}


/* paint terrain, creates neighbours if not already */
void map_paint(struct Map *m, enum TERRAIN t)
{
    if (map_curr_terrain(m) == TERRAIN_UNKNOWN) {
        map_create_neighbours(m);
    }
    hex_set_terrain(m->curr, t);
}
