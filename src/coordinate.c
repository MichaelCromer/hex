#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "hdr/coordinate.h"

const struct Coordinate COORDINATE_ORIGIN = { 0, 0, 0, 0 };
const struct Coordinate COORDINATE_DELTA_EE = { 1, 0, -1, 0 };
const struct Coordinate COORDINATE_DELTA_NE = { 1, -1, 0, 0 };
const struct Coordinate COORDINATE_DELTA_NW = { 0, -1, 1, 0 };
const struct Coordinate COORDINATE_DELTA_WW = { -1, 0, 1, 0 };
const struct Coordinate COORDINATE_DELTA_SW = { -1, 1, 0, 0 };
const struct Coordinate COORDINATE_DELTA_SE = { 0, 1, -1, 0 };

struct Coordinate coordinate(int p, int q, int r, int m)
{
    return (struct Coordinate) {p, q, r, m};
}

struct Coordinate coordinate_origin(void)
{
    return COORDINATE_ORIGIN;
}

struct Coordinate coordinate_delta(enum DIRECTION d)
{
    switch (d) {
        case DIRECTION_EE:
            return COORDINATE_DELTA_EE;
        case DIRECTION_NE:
            return COORDINATE_DELTA_NE;
        case DIRECTION_NW:
            return COORDINATE_DELTA_NW;
        case DIRECTION_WW:
            return COORDINATE_DELTA_WW;
        case DIRECTION_SW:
            return COORDINATE_DELTA_SW;
        case DIRECTION_SE:
            return COORDINATE_DELTA_SE;
        default:
            break;
    }
    return COORDINATE_ORIGIN;
}


enum CHILDREN coordinate_index(struct Coordinate c)
{
    return (((3*c.p + c.q) % 9) + 9) % 9;
}


struct Coordinate coordinate_lift_to(struct Coordinate c, unsigned int m)
{
    struct Coordinate lift = c;
    while (lift.m < m) {
        lift.p = round(lift.p / 3.0f);
        lift.q = round(lift.q / 3.0f);
        lift.r = -(lift.p + lift.q);
        lift.m = lift.m + 1;
    }
    return lift;
}

struct Coordinate coordinate_lift_by(struct Coordinate c, unsigned int m)
{
    return coordinate_lift_to(c, c.m + m);
}

struct Coordinate coordinate_drop(struct Coordinate c, enum CHILDREN i)
{
    struct Coordinate drop = c;
    drop.p = (3*drop.p) + ((4 < i && i < 8) ? -1 : ((1 < i && i < 5) ? 1 : 0));
    drop.q = (3*drop.q) + ((i % 3 == 2) ? -1 : (int)(i % 3));
    drop.r = -(drop.p + drop.q);
    drop.m = drop.m - 1;
    return drop;
}

bool coordinate_equals(struct Coordinate c1, struct Coordinate c2)
{
    return ((c1.p == c2.p) && (c1.q == c2.q) && (c1.r == c2.r) && (c1.m == c2.m));
}

bool coordinate_related(struct Coordinate c1, struct Coordinate c2)
{
    struct Coordinate lower = (c1.m < c2.m) ? c1 : c2; 
    struct Coordinate upper = (c1.m < c2.m) ? c2 : c1;
    lower = coordinate_lift_to(lower, upper.m);
    return coordinate_equals(lower, upper);
}

struct Coordinate coordinate_common_ancestor(struct Coordinate c1, struct Coordinate c2)
{
    if (c1.m < c2.m) { c1 = coordinate_lift_to(c1, c2.m); } 
    if (c2.m < c1.m) { c2 = coordinate_lift_to(c2, c1.m); }

    while (!coordinate_equals(c1, c2)) {
        c1 = coordinate_lift_by(c1, 1);
        c2 = coordinate_lift_by(c2, 1);
    }

    return c1;
}

struct Coordinate coordinate_add(struct Coordinate c1, struct Coordinate c2)
{
    struct Coordinate sum = { 0 };
    int e1 = pow(3, c1.m), e2 = pow(3, c2.m);
    sum.p = e1*c1.p + e2*c2.p;
    sum.q = e1*c1.q + e2*c2.q;
    sum.r = e1*c1.r + e2*c2.r;
    sum.m = (c1.m < c2.m) ? c1.m : c2.m;

    return sum;
}

struct Coordinate coordinate_scale(struct Coordinate c, int s)
{
    return (struct Coordinate) { s*c.p, s*c.q, s*c.r, c.m };
}

struct Coordinate coordinate_shift(struct Coordinate c, enum DIRECTION d)
{
    return coordinate_add(c, coordinate_delta(d));
}

struct Coordinate coordinate_nshift(struct Coordinate c, enum DIRECTION d, int n)
{
    return coordinate_add(c, coordinate_scale(coordinate_delta(d), n));
}

unsigned int coordinate_m(struct Coordinate c)
{
    return c.m;
}

int coordinate_p(struct Coordinate c)
{
    return c.p;
}

int coordinate_q(struct Coordinate c)
{
    return c.q;
}

int coordinate_r(struct Coordinate c)
{
    return c.r;
}
