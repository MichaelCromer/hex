#include <stdlib.h>
#include <stdbool.h>

#include "queue.h"
#include "grid.h"


struct Coordinate {
    int p, q, r;
};

struct Hex {
    struct Coordinate *c;
    enum TERRAIN t;
    struct Hex *n[6];
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

struct Hex *hex_create(void)
{
    struct Hex *h = malloc(sizeof(struct Hex));
    if (h == NULL) {
        return NULL;
    }

    h->c = malloc(sizeof(struct Coordinate));
    if (h->c == NULL) {
        free(h);
        return NULL;
    }

    (h->c)->p = 0;
    (h->c)->q = 0;
    (h->c)->r = 0;
    h->t = NONE;
    for (int i=0; i<6; i++) {
        h->n[i] = NULL;
    }

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
    h->t = t;
    return;
}


enum TERRAIN hex_terrain(struct Hex *h)
{
    return h->t;
}


struct Hex *hex_neighbour(struct Hex *h, enum DIRECTION d)
{
    return h->n[d];
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
    coordinate_delta(h->c, d, nbr->c);

    /* assign the new neighbour */
    (h->n)[d] = nbr;
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
        if (h->n[i] != NULL) {
            continue;
        }
        hex_create_neighbour(h, i);
    }

    return;
}
