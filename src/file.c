#include <stdio.h>

#include "include/atlas.h"
#include "include/coordinate.h"
#include "include/tile.h"
#include "include/write.h"


void write_coordinate(FILE *file, const struct Coordinate *c)
{
    if (!file || !c) {
        return;
    }

    int p = coordinate_p(c),
        q = coordinate_q(c),
        r = coordinate_r(c),
        m = coordinate_m(c);
    fprintf(file, "%d,%d,%d,%d", p, q, r, m);
}


void write_location(FILE *file, const struct Location *location)
{
    if (!file || !location) {
        return;
    }

    write_coordinate(file, location_coordinate(location));
    fprintf(file, ":");
    fprintf(file, "%d", location_type(location));
}


void write_tile(FILE *file, const struct Tile *tile)
{
    if (!file || !tile) {
        return;
    }

    fprintf(file, "%d", tile_seed(tile));
    fprintf(file, ":");
    fprintf(file, "%d", tile_terrain(tile));
    fprintf(file, ":");

    for (int i = 0; i < NUM_DIRECTIONS; i++) {
        fprintf(file, "%d", tile_road(tile, i));
        fprintf(file, ",");
    }

    fprintf(file, ":");

    for (int i = 0; i < NUM_DIRECTIONS; i++) {
        fprintf(file, "%d", tile_river(tile, i));
        fprintf(file, ",");
    }
}


void write_chart(FILE *file, const struct Chart *chart)
{
    if (!file || !chart) {
        return;
    }

    write_coordinate(file, chart_coordinate(chart));
    if (chart_tile(chart)) {
        fprintf(file, ":");
        write_tile(file, chart_tile(chart));
    }
    fprintf(file, "\n");

    if (!chart_children(chart)) {
        return;
    }

    for (int i = 0; i < NUM_CHILDREN; i++) {
        write_chart(file, chart_child(chart, i));
    }
}

void write_directory(FILE *file, struct Directory *directory)
{
    struct Directory *curr = directory;

    while (curr) {
        write_location(file, directory_location(curr));
        fprintf(file, "\n");
        curr = directory_next(curr);
    }
}

void write_atlas(FILE *file, const struct Atlas *atlas)
{
    if (!file || !atlas) {
        return;
    }

    fprintf(file, "===ROOT===\n");
    write_chart(file, atlas_root(atlas));
    fprintf(file, "===CURR===\n");
    write_coordinate(file, atlas_coordinate(atlas));
    fprintf(file, "\n");
    fprintf(file, "===LOCN===\n");
    write_directory(file, atlas_directory(atlas));
}

void write_state(FILE *file, struct State *state)
{
    if (!file || !state) {
        return;
    }

    write_atlas(file, state_atlas(state));

    return;
}
