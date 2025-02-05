#ifndef COORDINATE_H
#define COORDINATE_H

#include <stdbool.h>

#include "enum.h"

struct Coordinate {
    int p, q, r;
    unsigned int m;
};

struct Coordinate coordinate(int p, int q, int r, int m);
enum CHILDREN coordinate_index(struct Coordinate c);
struct Coordinate coordinate_origin(void);
struct Coordinate coordinate_delta(enum DIRECTION d);
struct Coordinate coordinate_lift_to(struct Coordinate c, unsigned int m);
struct Coordinate coordinate_lift_by(struct Coordinate c, unsigned int m);
struct Coordinate coordinate_drop(struct Coordinate c, enum CHILDREN i);
bool coordinate_equals(struct Coordinate c1, struct Coordinate c2);
bool coordinate_related(struct Coordinate c1, struct Coordinate c2);
struct Coordinate coordinate_common_ancestor(struct Coordinate c1, struct Coordinate c2);
struct Coordinate coordinate_add(struct Coordinate c1, struct Coordinate c2);
struct Coordinate coordinate_scale(struct Coordinate c, int s);
struct Coordinate coordinate_shift(struct Coordinate c, enum DIRECTION d);
struct Coordinate coordinate_nshift(struct Coordinate c, enum DIRECTION d, int n);
unsigned int coordinate_m(struct Coordinate c);
int coordinate_p(struct Coordinate c);
int coordinate_q(struct Coordinate c);
int coordinate_r(struct Coordinate c);

#endif
