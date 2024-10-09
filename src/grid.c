#include <stdlib.h>
#include <stdbool.h>

#include "queue.h"
#include "grid.h"


struct Coordinate {
    int p, q, r;
};

struct Hex {
    unsigned int magnitude;
    struct Coordinate *coordinate;
    enum TERRAIN terrain;
    struct Hex *parent;
    struct Hex *neighbours[6];
    struct Hex **children;
};

/*
 *      COORDINATE Functions
 */

/* coordinate deltas in each hex direction */
static const struct Coordinate d_ee = {  1,  0, -1 };
static const struct Coordinate d_ne = {  1, -1,  0 };
static const struct Coordinate d_nw = {  0, -1,  1 };
static const struct Coordinate d_ww = { -1,  0,  1 };
static const struct Coordinate d_sw = { -1,  1,  0 };
static const struct Coordinate d_se = {  0,  1, -1 };


struct Coordinate *coordinate_create(int p, int q, int r)
{
    struct Coordinate *c = malloc(sizeof(struct Coordinate));
    if (c == NULL) {
        return NULL;
    }

    c->p = p;
    c->q = q;
    c->r = r;

    return c;
}


void coordinate_destroy(struct Coordinate *c)
{
    free(c);
    c = NULL;
}


enum DIRECTION direction_opposite(enum DIRECTION d)
{
    return (d + 3) % 6;
}


void coordinate_parent(const struct Coordinate *c, struct Coordinate *a)
{
    a->p = 3 * round(c->p / 3.0f);
    a->q = 3 * round(c->q / 3.0f);
    a->r = -(a->p + a->q);

    return;
}


bool coordinate_equals(struct Coordinate *c0, struct Coordinate *c1)
{
    return ((c0->p == c1->p) && (c0->q == c1->q) && (c0->r == c1->r));
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
    a->p = c1->p + c2->p;
    a->q = c1->q + c2->q;
    a->r = c1->r + c2->r;

    return;
}


void coordinate_delta(struct Coordinate *c, enum DIRECTION d, struct Coordinate *a)
{
    switch (d) {
        case EAST:
            coordinate_add(c, &d_ee, a);
            break;
        case NORTHEAST:
            coordinate_add(c, &d_ne, a);
            break;
        case NORTHWEST:
            coordinate_add(c, &d_nw, a);
            break;
        case WEST:
            coordinate_add(c, &d_ww, a);
            break;
        case SOUTHWEST:
            coordinate_add(c, &d_sw, a);
            break;
        case SOUTHEAST:
            coordinate_add(c, &d_se, a);
            break;
    }
    return;
}

/*
 *      HEX Functions
 */

struct Hex *hex_create(unsigned int m, int p, int q, int r)
{
    /* try allocate hex */
    struct Hex *h = malloc(sizeof(struct Hex));
    if (h == NULL) {
        return NULL;
    }

    /* try allocate coordinate */
    h->coordinate = coordinate_create((m+1)*p, (m+1)*q, (m+1)*r)
    if (h->coordinate == NULL) {
        free(h);
        return NULL;
    }

    /* try allocate parent, neighbour, children */
    h->parent = NULL;
    for (int i=0; i<6; i++) {
        h->neighbours[i] = NULL;
    }
    if (m > 0) {
        h->children = malloc(9*sizeof(struct Hex *));
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
    h->magnitude = m;
    h->terrain = NONE;

    return h;
}


void hex_destroy(struct Hex *h)
{
    /* early exit allows 'recursive step' */
    if (h == NULL) {
        return;
    }

    /* collect pointers to all the connected hexes */
    struct Queue *open = queue_create(h, 0);
    struct Queue *closed = NULL;
    struct Hex *curr = NULL, *nbr = NULL;
    while (open) {
        curr = queue_hex(open);
        queue_remove(&open, curr);
        queue_add(&closed, curr, 0);

        for (int i=0; i<6; i++) {
            nbr = hex_neighbour(h, i);
            if (!queue_find(open, nbr) && !queue_find(closed, nbr)) {
                queue_add(&open, nbr, 0);
            }
        }
    }

    /* destroy each collected hex */
    while (closed) {
        curr = queue_hex(closed);
        queue_remove(&closed, curr);

        coordinate_destroy(curr->c);
        free(curr);
        curr = NULL;
    }

    return;
}


void hex_insert_below(struct Hex **root, struct Hex *new)
{
    /* handle pathological cases */
    if (!root || !(*root) || !new) {
        return;
    }
    if (new->magnitude >= root->magnitude) {
        return;
    }

    /* figure out where to look next */
    struct Coordinate c = { 0, 0, 0 };
    unsigned int d_mag = root->magnitude - new->magnitude;
    for 

}



float hex_u(struct Hex *hex)
{
    return ROOT3 * (hex->c->p + hex->c->q/2.0f);
}


float hex_v(struct Hex *hex)
{
    return 1.5f * hex->c->q;
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


struct Hex *hex_neighbour(struct Hex *h, enum DIRECTION d)
{
    return h->neighbours[d];
}


struct Hex *hex_find(struct Hex *start, struct Coordinate *target)
{
    struct Queue *open = NULL;
    struct Queue *closed = NULL;
    struct Hex *end = NULL; /* might not exist */

    /* start with the current hex */
    queue_add(&open, start, coordinate_distance(start->c, target));

    while (open) {
        struct Hex *curr = queue_hex(open);

        /* we found it, end here */
        if (coordinate_equals(curr->c, target)) {
            end = curr;
            break;
        }

        /* open each unclosed non-null neighbour */
        for (int i=0; i<6; i++) {
            struct Hex *nbr = (curr->n)[i];

            /* skip if null or already closed */
            if (nbr == NULL || queue_find(closed, nbr)) {
                continue;
            }

            queue_add(&open, nbr, coordinate_distance(nbr->c, target));
        }

        /* close the current open head */
        queue_add(&closed, curr, 0);
        queue_remove(&open, curr);

    }

    /* we did it, let's clean up and get out of here */
    queue_destroy(open);
    queue_destroy(closed);
    return end; /* NULL if target not found */
}


void hex_create_neighbour(struct Hex *h, enum DIRECTION d)
{
    /* create the hex with the right coords */
    struct Hex *nbr = hex_create();
    coordinate_delta(h->coordinate, d, nbr->c);

    /* assign the new neighbour */
    (h->neighbours)[d] = nbr;
    nbr->n[direction_opposite(d)] = h;

    /* find the neighbour's neighbourhood */
    struct Coordinate dc = { 0, 0, 0 };
    for (int i = 0; i < 6; i++) {
        coordinate_delta(nbr->c, i, &dc);
        struct Hex *nbr_nbr = hex_find(nbr, &dc);
        if (nbr_nbr) {
            nbr->n[i] = nbr_nbr;
            nbr_nbr->n[direction_opposite(i)] = nbr;
        }
    }

    return;
}


void hex_create_neighbours(struct Hex *h)
{
    for (int i=0; i<6; i++) {
        if (h->neighbours[i] != NULL) {
            continue;
        }
        struct Hex *new = hex_create(
        hex_create_neighbour(h, i);
    }

    return;
}
