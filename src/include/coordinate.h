#ifndef COORDINATE_H
#define COORDINATE_H

#include <stdbool.h>

#include "enum.h"

struct Coordinate;

struct Coordinate *coordinate_duplicate(const struct Coordinate *c);
void coordinate_destroy(struct Coordinate *c);
struct Coordinate *coordinate_create_ancestor(const struct Coordinate *c0,
                                              const struct Coordinate *c1);
struct Coordinate *coordinate_create_parent(const struct Coordinate *c);
struct Coordinate *coordinate_create_origin(void);
const struct Coordinate *coordinate_origin(void);
const struct Coordinate *coordinate_delta(enum DIRECTION d);
enum CHILDREN coordinate_index(const struct Coordinate *c);
unsigned int coordinate_m(const struct Coordinate *c);
int coordinate_p(const struct Coordinate *c);
int coordinate_q(const struct Coordinate *c);
int coordinate_r(const struct Coordinate *c);
bool coordinate_equals(const struct Coordinate *c0, const struct Coordinate *c1);
bool coordinate_related(const struct Coordinate *c1, const struct Coordinate *c2);
void coordinate_child(const struct Coordinate *c,
                      enum CHILDREN i, struct Coordinate *ch);
void coordinate_add(const struct Coordinate *c1,
                    const struct Coordinate *c2, struct Coordinate *a);
void coordinate_shift(struct Coordinate *c, enum DIRECTION d);
void coordinate_nshift(struct Coordinate *c, enum DIRECTION d, int n);
void coordinate_common_ancestor(const struct Coordinate *c0,
                                const struct Coordinate *c1, struct Coordinate *a);

#endif
