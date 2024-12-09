#include <stdlib.h>

#include "include/location.h"


struct Location {
    struct Coordinate *c;
    enum LOCATION type;
};


struct Location *location_create(const struct Coordinate *c)
{
    struct Location *location = malloc(sizeof(struct Location));

    location->c = coordinate_duplicate(c);
    location->type = LOCATION_NONE;

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
