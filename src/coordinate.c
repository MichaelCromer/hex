#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

#include "include/coordinate.h"

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

/*
 *      Coordinate Functions
 */

const struct Coordinate COORDINATE_ZERO = {0,0,0,0};
const struct Coordinate COORDINATE_DELTA_EE = {1,0,-1,0};
const struct Coordinate COORDINATE_DELTA_NE = {1,-1,0,0};
const struct Coordinate COORDINATE_DELTA_NW = {0,-1,1,0};
const struct Coordinate COORDINATE_DELTA_WW = {-1,0,1,0};
const struct Coordinate COORDINATE_DELTA_SW = {-1,1,0,0};
const struct Coordinate COORDINATE_DELTA_SE = {0,1,-1,0};


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


const struct Coordinate *coordinate_zero(void)
{
    return &COORDINATE_ZERO;
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


bool coordinate_equals(const struct Coordinate *c0, const struct Coordinate *c1)
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


const struct Coordinate *coordinate_delta(enum DIRECTION d)
{
    const struct Coordinate *delta;
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
    const struct Coordinate *delta = coordinate_delta(d);
    c->p += delta->p;
    c->q += delta->q;
    c->r += delta->r;

    return;
}


unsigned int coordinate_magnitude(const struct Coordinate *c)
{
    return c->m;
}


int coordinate_p(const struct Coordinate *c)
{
    return c->p;
}


int coordinate_q(const struct Coordinate *c)
{
    return c->q;
}


int coordinate_r(const struct Coordinate *c)
{
    return c->r;
}
