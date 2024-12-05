#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "include/coordinate.h"


struct Coordinate {
    int p, q, r;
    unsigned int m;
};


const struct Coordinate COORDINATE_ORIGIN = {0,0,0,0};
const struct Coordinate COORDINATE_DELTA_EE = {1,0,-1,0};
const struct Coordinate COORDINATE_DELTA_NE = {1,-1,0,0};
const struct Coordinate COORDINATE_DELTA_NW = {0,-1,1,0};
const struct Coordinate COORDINATE_DELTA_WW = {-1,0,1,0};
const struct Coordinate COORDINATE_DELTA_SW = {-1,1,0,0};
const struct Coordinate COORDINATE_DELTA_SE = {0,1,-1,0};


struct Coordinate *coordinate_create(int p, int q, int r, int m)
{
    struct Coordinate *c = malloc(sizeof(struct Coordinate));

    c->p = p;
    c->q = q;
    c->r = r;
    c->m = m;

    return c;
}


void coordinate_destroy(struct Coordinate *c)
{
    free(c);
}


void coordinate_copy(const struct Coordinate *c0, struct Coordinate *c1)
{
    if (!c0 || !c1) {
        return;
    }
    c1->p = c0->p;
    c1->q = c0->q;
    c1->r = c0->r;
    c1->m = c0->m;
}


struct Coordinate *coordinate_duplicate(const struct Coordinate *c)
{
    if (!c) {
        return NULL;
    }
    struct Coordinate *d = coordinate_create(0,0,0,0);
    coordinate_copy(c, d);
    return d;
}


const struct Coordinate *coordinate_origin(void)
{
    return &COORDINATE_ORIGIN;
}


struct Coordinate *coordinate_create_origin(void)
{
    return coordinate_duplicate(coordinate_origin());
}


const struct Coordinate *coordinate_delta(enum DIRECTION d)
{
    const struct Coordinate *delta = NULL;
    switch (d) {
        case DIRECTION_EE:
            delta = &COORDINATE_DELTA_EE;
            break;
        case DIRECTION_NE:
            delta = &COORDINATE_DELTA_NE;
            break;
        case DIRECTION_NW:
            delta = &COORDINATE_DELTA_NW;
            break;
        case DIRECTION_WW:
            delta = &COORDINATE_DELTA_WW;
            break;
        case DIRECTION_SW:
            delta = &COORDINATE_DELTA_SW;
            break;
        case DIRECTION_SE:
            delta = &COORDINATE_DELTA_SE;
            break;
        default:
            delta = &COORDINATE_ORIGIN;
            break;
    }
    return delta;
}


void coordinate_lift_to(struct Coordinate *c, unsigned int m)
{
    while (c->m < m) {
        c->p = round(c->p / 3.0f);
        c->q = round(c->q / 3.0f);
        c->r = -(c->p + c->q);
        c->m = c->m + 1;
    }
}


void coordinate_lift_by(struct Coordinate *c, unsigned int m)
{
    for (unsigned int i = 0; i < m; i++) {
        coordinate_lift_to(c, c->m + 1);
    }
}


void coordinate_drop_to(struct Coordinate *c, enum CHILDREN i, unsigned int m)
{
    while (coordinate_m(c) > m) {
        c->p = (3 * c->p) + ((4 < i && i < 8) ? -1 : ((1 < i && i < 5) ? 1 : 0));
        c->q = (3 * c->q) + ((i % 3 == 2) ? -1 : (int)(i % 3));
        c->r = -(c->p + c->q);
        c->m = c->m - 1;
    }
}

void coordinate_drop_by(struct Coordinate *c, enum CHILDREN ch, unsigned int m)
{
    for (unsigned int i = 0; i < m; i++) {
        coordinate_drop_to(c, ch, coordinate_m(c) - 1);
    }
}


void coordinate_parent(const struct Coordinate *c, struct Coordinate *p)
{
    coordinate_copy(c, p);
    coordinate_lift_by(p, 1);
}


void coordinate_child(const struct Coordinate *c,
        enum CHILDREN i,
        struct Coordinate *ch)
{
    coordinate_copy(c, ch);
    coordinate_drop_by(ch, i, 1);
}


struct Coordinate *coordinate_create_parent(const struct Coordinate *c)
{
    struct Coordinate *p = coordinate_create_origin();
    coordinate_parent(c, p);
    return p;
}


bool coordinate_equals(const struct Coordinate *c0, const struct Coordinate *c1)
{
    return ((c0->p == c1->p) &&
            (c0->q == c1->q) &&
            (c0->r == c1->r) &&
            (c0->m == c1->m)
           );
}


bool coordinate_related(const struct Coordinate *c1, const struct Coordinate *c2)
{
    struct Coordinate lower = COORDINATE_ORIGIN;
    struct Coordinate upper = COORDINATE_ORIGIN;
    coordinate_copy( (c1->m <  c2->m) ? c1 : c2, &lower);
    coordinate_copy( (c1->m >= c2->m) ? c1 : c2, &upper);

    coordinate_lift_to(&lower, upper.m);
    return coordinate_equals(&lower, &upper);
}


void coordinate_common_ancestor(
        const struct Coordinate *c0,
        const struct Coordinate *c1,
        struct Coordinate *a)
{
    struct Coordinate *tmp0 = coordinate_duplicate(c0);
    struct Coordinate *tmp1 = coordinate_duplicate(c1);
    struct Coordinate *lower = (tmp0->m < tmp1->m) ? tmp0 : tmp1;

    while (!coordinate_equals(tmp0, tmp1)) {
        lower = (tmp0->m < tmp1->m) ? tmp0 : tmp1;
        coordinate_lift_by(lower, 1);
    }

    coordinate_copy(lower, a);
    coordinate_destroy(tmp0);
    coordinate_destroy(tmp1);
}


struct Coordinate *coordinate_create_ancestor(
        const struct Coordinate *c0,
        const struct Coordinate *c1)
{
    struct Coordinate *a = coordinate_create_origin();
    coordinate_common_ancestor(c0, c1, a);
    return a;
}


enum CHILDREN coordinate_index(const struct Coordinate *c)
{
    return (((3*c->p + c->q) % 9) + 9) % 9;
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


void coordinate_shift(struct Coordinate *c, enum DIRECTION d)
{
    const struct Coordinate *delta = coordinate_delta(d);
    c->p += delta->p;
    c->q += delta->q;
    c->r += delta->r;
}


void coordinate_nshift(struct Coordinate *c, enum DIRECTION d, int n)
{
    const struct Coordinate *delta = coordinate_delta(d);
    c->p += n * delta->p;
    c->q += n * delta->q;
    c->r += n * delta->r;
}


unsigned int coordinate_m(const struct Coordinate *c)
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
