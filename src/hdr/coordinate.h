#ifndef COORDINATE_H
#define COORDINATE_H

#include <stdbool.h>
#include <stdint.h>

#include "enum.h"

struct Coordinate
{
    int32_t p, q, r;
    uint32_t m;
};


struct Coordinate coordinate(int32_t p, int32_t q, int32_t r, uint32_t m);
enum CHILDREN coordinate_index(struct Coordinate c);
struct Coordinate coordinate_origin(void);
struct Coordinate coordinate_delta(enum DIRECTION d);
struct Coordinate coordinate_lift_to(struct Coordinate c, uint32_t m);
struct Coordinate coordinate_lift_by(struct Coordinate c, uint32_t m);
struct Coordinate coordinate_drop(struct Coordinate c, enum CHILDREN i);
bool coordinate_equals(struct Coordinate c1, struct Coordinate c2);
bool coordinate_related(struct Coordinate c1, struct Coordinate c2);
struct Coordinate coordinate_common_ancestor(struct Coordinate c1, struct Coordinate c2);
struct Coordinate coordinate_add(struct Coordinate c1, struct Coordinate c2);
struct Coordinate coordinate_scale(struct Coordinate c, int s);
struct Coordinate coordinate_shift(struct Coordinate c, enum DIRECTION d);
struct Coordinate coordinate_nshift(struct Coordinate c, enum DIRECTION d, int n);
uint32_t coordinate_m(struct Coordinate c);
int32_t coordinate_p(struct Coordinate c);
int32_t coordinate_q(struct Coordinate c);
int32_t coordinate_r(struct Coordinate c);

#endif
