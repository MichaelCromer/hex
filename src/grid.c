#include <math.h>
#include <stdlib.h>
#include <stdbool.h>

#include "grid.h"


/*
 * Terrain lookups
 *
 */

static const char *terrain_none = "None";
static const char *terrain_water = "Water";
static const char *terrain_mountains = "Mountains";
static const char *terrain_plains = "Plains";
static const char *terrain_hills = "Hills";
static const char *terrain_forest = "Forset";
static const char *terrain_desert = "Desert";
static const char *terrain_jungle = "Jungle";
static const char *terrain_swamp = "Swamp";


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


/*
 *  struct Coordinate
 *
 *  (p, q, r) give position in hexagonal grid using cube coordinates
 *      => (p + q + r) == 0
 *
 *  m gives 'magnitude', or degree in nonary tree
 *
 */
struct Coordinate {
    int p, q, r;
    unsigned int m;
};

struct Hex {
    struct Coordinate *coordinate;
    enum TERRAIN terrain;
    struct Hex **children;
};


/*
 *      Coordinate Functions
 */

static const struct Coordinate COORDINATE_ZERO = {0,0,0,0};
static const struct Coordinate COORDINATE_DELTA_EE = {1,0,-1,0};
static const struct Coordinate COORDINATE_DELTA_NE = {1,-1,0,0};
static const struct Coordinate COORDINATE_DELTA_NW = {0,-1,1,0};
static const struct Coordinate COORDINATE_DELTA_WW = {-1,0,1,0};
static const struct Coordinate COORDINATE_DELTA_SW = {-1,1,0,0};
static const struct Coordinate COORDINATE_DELTA_SE = {0,1,-1,0};


struct Coordinate *coordinate_create(int p, int q, int r, int m)
{
    struct Coordinate *c = malloc(sizeof(struct Coordinate));
    if (c == NULL) {
        return NULL;
    }

    c->p = p;
    c->q = q;
    c->r = r;
    c->m = m;

    return c;
}


void coordinate_destroy(struct Coordinate *c)
{
    free(c);
    c = NULL;
}


void coordinate_copy(const struct Coordinate *c, struct Coordinate *a)
{
    a->p = c->p;
    a->q = c->q;
    a->r = c->r;
    a->m = c->m;
}


struct Coordinate *coordinate_duplicate(const struct Coordinate *c)
{
    struct Coordinate *d = coordinate_create(0,0,0,0);
    if (!d) {
        return NULL;
    }
    coordinate_copy(c, d);
    return d;
}


struct Coordinate *coordinate_zero()
{
    return coordinate_create(0,0,0,0);
}


/* TODO direction_opposite of dubious utility? */
enum DIRECTION direction_opposite(enum DIRECTION d)
{
    return (d + 3) % 6;
}


/* coordinate_lift(struct Coordinate *c, unsigned int m)
 *
 *  lift a coordinate into the mth layer
 */
void coordinate_lift_to(struct Coordinate *c, unsigned int m)
{
    while (c->m < m) {
        c->p = round(c->p / 3.0f);
        c->q = round(c->q / 3.0f);
        c->r = -(c->p + c->q);
        c->m = c->m + 1;
    }

    return;
}


void coordinate_lift_by(struct Coordinate *c, unsigned int m)
{
    for (unsigned int i=0; i<m; i++) {
        coordinate_lift_to(c, c->m + 1);
    }
}


void coordinate_parent(const struct Coordinate *c, struct Coordinate *p)
{
    coordinate_copy(c, p);
    coordinate_lift_by(p, 1);
}


bool coordinate_equals(struct Coordinate *c0, struct Coordinate *c1)
{
    return ((c0->p == c1->p) &&
            (c0->q == c1->q) &&
            (c0->r == c1->r) &&
            (c0->m == c1->m)
           );
}

/* coordinate_related(const struct Coordinate *c1, const struct Coordinate *c2)
 *
 *  determine if c1 and c2 are related within the layer hierarchy
 */
bool coordinate_related(const struct Coordinate *c1, const struct Coordinate *c2)
{
    struct Coordinate lower = COORDINATE_ZERO;
    struct Coordinate upper = COORDINATE_ZERO;
    coordinate_copy( (c1->m <  c2->m) ? c1 : c2, &lower);
    coordinate_copy( (c1->m >= c2->m) ? c1 : c2, &upper);

    /* lift the lower to the height of the upper */
    coordinate_lift_to(&lower, upper.m);
    return coordinate_equals(&lower, &upper);
}


void coordinate_common_ancestor(
        const struct Coordinate *c1,
        const struct Coordinate *c2,
        struct Coordinate *a)
{
    if (coordinate_related(c1, c2)) {
        coordinate_copy((c1->m > c2->m) ? c1 : c2, a);
        return;
    }

    struct Coordinate tmp1 = COORDINATE_ZERO;
    struct Coordinate tmp2 = COORDINATE_ZERO;
    coordinate_copy(c1, &tmp1);
    coordinate_copy(c2, &tmp2);

    while (!coordinate_equals(&tmp1, &tmp2)) {
        struct Coordinate *lower = (tmp1.m < tmp2.m) ? &tmp1 : &tmp2;
        coordinate_lift_by(lower, 1);
    }

    coordinate_copy(&tmp1, a);
    return;
}


/* coordinate_index(const struct Coordinate *c)
 *
 *  get the 'transverse index' of the coordinate in its parent's children array
 */
int coordinate_index(const struct Coordinate *c)
{
    return (((3*c->p + c->q) % 9) + 9) % 9;
}


int coordinate_distance(struct Coordinate *c0, struct Coordinate *c1)
{
    int dp = abs(c0->p - c1->p);
    int dq = abs(c0->q - c1->q);
    int dr = abs(c0->r - c1->r);

    int dc = 0;
    dc = (dp > dq) ? dp : dq;
    dc = (dr > dc) ? dr : dc;

    return dc;
}


void coordinate_add(
        const struct Coordinate *c1,
        const struct Coordinate *c2,
        struct Coordinate *a
    )
{
    int e1 = pow(3, c1->m), e2 = pow(3, c2->m);
    a->p = e1*c1->p + e2*c2->p;
    a->q = e1*c1->q + e2*c2->q;
    a->r = e1*c1->r + e2*c2->r;
    a->m = (c1->m < c2->m) ? c1->m : c2->m;

    return;
}


static const struct Coordinate *coordinate_delta(enum DIRECTION d)
{
    static const struct Coordinate *delta;
    switch (d) {
        case EAST:
            delta = &COORDINATE_DELTA_EE;
            break;
        case NORTHEAST:
            delta = &COORDINATE_DELTA_NE;
            break;
        case NORTHWEST:
            delta = &COORDINATE_DELTA_NW;
            break;
        case WEST:
            delta = &COORDINATE_DELTA_WW;
            break;
        case SOUTHWEST:
            delta = &COORDINATE_DELTA_SW;
            break;
        case SOUTHEAST:
            delta = &COORDINATE_DELTA_SE;
            break;
        default:
            delta = &COORDINATE_ZERO;
            break;
    }

    return delta;
}


void coordinate_shift(struct Coordinate *c, enum DIRECTION d)
{
    struct Coordinate tmp = COORDINATE_ZERO;
    const struct Coordinate *delta = coordinate_delta(d);
    coordinate_add(c, delta, &tmp);
    coordinate_copy(&tmp, c);

    return;
}


/*
 *      HEX Functions
 */


struct Hex *hex_create(struct Coordinate *c)
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
    if (c->m > 0) {
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
    struct Coordinate origin = COORDINATE_ZERO;
    return hex_create(&origin);
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
    return hex->coordinate->m;
}


int hex_p(struct Hex *hex)
{
    return hex->coordinate->p;
}


int hex_q(struct Hex *hex)
{
    return hex->coordinate->q;
}


int hex_r(struct Hex *hex)
{
    return hex->coordinate->r;
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
    struct Coordinate zoom = COORDINATE_ZERO;
    coordinate_copy(target, &zoom);
    coordinate_lift_to(&zoom, root->coordinate->m - 1);

    /* recurse to find within zoomed block */
    return hex_find(
            hex_child(root, coordinate_index(&zoom)),
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
        struct Coordinate ancestor = COORDINATE_ZERO;
        coordinate_common_ancestor(origin, target, &ancestor);
        struct Hex *top = hex_create(&ancestor);
        hex_insert(&top, *root);
        hex_insert(&top, hex);
        *root = top;
        return;
    }

    if (hex_find(*root, target)) {
        return;
    }

    struct Coordinate above = COORDINATE_ZERO;
    coordinate_parent(target, &above);
    struct Hex *parent = hex_find(*root, &above);

    if (!parent) {
        parent = hex_create(&above);
        hex_insert(root, parent);
    }

    hex_set_child(parent, coordinate_index(target), hex);

    return;
}


struct Hex *hex_neighbour(struct Hex *root, struct Hex *hex, enum DIRECTION d)
{
    struct Coordinate target = COORDINATE_ZERO;
    coordinate_add(hex->coordinate, coordinate_delta(d), &target);

    return hex_find(root, &target);
}


void hex_create_neighbours(struct Hex **root, struct Hex *hex)
{
    struct Coordinate location = COORDINATE_ZERO;
    static const struct Coordinate *d_loc;
    struct Hex *neighbour = NULL;
    for (int i=0; i<6; i++) {
        d_loc = coordinate_delta(i);
        coordinate_add(hex->coordinate, d_loc, &location);
        neighbour = hex_find(*root, &location);
        if (!neighbour) {
            neighbour = hex_create(&location);
            hex_insert(root, neighbour);
        }
    }

    return;
}

