#include <stdlib.h>

#include "include/location.h"

struct Location {
    struct Coordinate *c;
    enum LOCATION type;
};

struct Location *location_create(const struct Coordinate *c, enum LOCATION t)
{
    struct Location *location = malloc(sizeof(struct Location));

    location->c = coordinate_duplicate(c);
    location->type = t;

    return location;
}

void location_destroy(struct Location *location)
{
    if (location->c) {
        coordinate_destroy(location->c);
        location->c = NULL;
    }

    free(location);
}

enum LOCATION location_type(const struct Location *location)
{
    return location->type;
}

void location_set_type(struct Location *location, enum LOCATION l)
{
    location->type = l;
}

struct Coordinate *location_coordinate(const struct Location *location)
{
    return location->c;
}

struct Directory {
    struct Location *location;
    struct Directory *next;
};

struct Directory *directory_create(struct Location *location)
{
    struct Directory *directory = malloc(sizeof(struct Directory));

    directory->location = location;
    directory->next = NULL;

    return directory;
}

void directory_destroy(struct Directory *directory)
{
    if (!directory) {
        return;
    }

    directory_destroy(directory->next);
    directory->next = NULL;

    location_destroy(directory->location);
    directory->location = NULL;

    free(directory);
}

void directory_insert(struct Directory **directory, struct Location *location)
{
    struct Directory *new = directory_create(location);

    new->next = *directory;
    *directory = new;
}

struct Directory *directory_next(const struct Directory *directory)
{
    return directory->next;
}

struct Location *directory_location(const struct Directory *directory)
{
    return directory->location;
}
