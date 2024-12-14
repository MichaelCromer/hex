#ifndef LOCATION_H
#define LOCATION_H

#include "coordinate.h"
#include "enum.h"

struct Location;
struct Location *location_create(const struct Coordinate *c, enum LOCATION t);
void location_destroy(struct Location *location);
enum LOCATION location_type(const struct Location *location);
void location_set_type(struct Location *location, enum LOCATION l);
struct Coordinate *location_coordinate(const struct Location *location);

struct Directory;
struct Directory *directory_create(struct Location *location);
void directory_destroy(struct Directory *directory);
void directory_insert(struct Directory **directory, struct Location *location);
struct Directory *directory_next(const struct Directory *directory);
struct Location *directory_location(const struct Directory *directory);

#endif
