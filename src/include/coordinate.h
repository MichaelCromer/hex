#ifndef COORDINATE_H
#define COORDINATE_H

#define NUM_DIRECTIONS 6

enum DIRECTION {
    EAST,
    NORTHEAST,
    NORTHWEST,
    WEST,
    SOUTHWEST,
    SOUTHEAST,
    DIRECTION_NONE
};

struct Coordinate;

/* memory managers */
struct Coordinate *coordinate_create(int p, int q, int r, int m);
struct Coordinate *coordinate_duplicate(const struct Coordinate *c);
void coordinate_destroy(struct Coordinate *c);

/* constants */
const struct Coordinate *coordinate_zero(void);
const struct Coordinate *coordinate_delta(enum DIRECTION d);

/* getters */
int coordinate_index(const struct Coordinate *c);
unsigned int coordinate_magnitude(const struct Coordinate *c);
int coordinate_p(const struct Coordinate *c);
int coordinate_q(const struct Coordinate *c);
int coordinate_r(const struct Coordinate *c);

/* testers */
bool coordinate_equals(const struct Coordinate *c0, const struct Coordinate *c1);
bool coordinate_related(const struct Coordinate *c1, const struct Coordinate *c2);

/* derivers */
void coordinate_copy(const struct Coordinate *c, struct Coordinate *a);
void coordinate_parent(const struct Coordinate *c, struct Coordinate *p);
void coordinate_common_ancestor(
        const struct Coordinate *c1,
        const struct Coordinate *c2,
        struct Coordinate *a);
void coordinate_add(
        const struct Coordinate *c1,
        const struct Coordinate *c2,
        struct Coordinate *a);

/* modifiers */
void coordinate_shift(struct Coordinate *c, enum DIRECTION d);
void coordinate_lift_to(struct Coordinate *c, unsigned int m);
void coordinate_lift_by(struct Coordinate *c, unsigned int m);

#endif
