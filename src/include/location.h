#ifndef LOCATION_H
#define LOCATION_H

#include "coordinate.h"
#include "enum.h"

struct Location;
struct Location *location_create(const struct Coordinate *c);
void location_destroy(struct Location *location);
enum LOCATION location_type(const struct Location *location);
void location_set_type(struct Location *location, enum LOCATION l);
struct Coordinate *location_coordinate(const struct Location *location);

#endif