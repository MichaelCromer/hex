#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "include/atlas.h"
#include "include/coordinate.h"
#include "include/tile.h"

struct Chart {
    struct Coordinate coordinate;
    struct Tile *tile;
    struct Chart **children;
};

struct Chart *chart_create(struct Coordinate c)
{
    struct Chart *chart = malloc(sizeof(struct Chart));

    chart->coordinate = c;
    chart->tile = NULL;
    chart->children = NULL;

    if (coordinate_m(c) == 0) {
        chart->tile = tile_create();
    } else {
        chart->children = malloc(NUM_CHILDREN * sizeof(struct Chart *));
        for (int i = 0; i < NUM_CHILDREN; i++) {
            chart->children[i] = NULL;
        }
    }

    return chart;
}

struct Chart *chart_create_ancestor(struct Chart *chart1, struct Chart *chart2)
{
    struct Coordinate a = coordinate_common_ancestor(
        chart_coordinate(chart1),
        chart_coordinate(chart2)
    );

    return chart_create(a);
}

struct Chart **chart_children(const struct Chart *chart)
{
    return chart->children;
}

struct Chart *chart_child(const struct Chart *chart, enum CHILDREN c)
{
    if (!chart || !chart->children) return NULL;
    return chart->children[c];
}

void chart_set_child(struct Chart *chart, enum CHILDREN c, struct Chart *child)
{
    if (!chart || chart_child(chart, c)) return;
    chart->children[c] = child;
}

void chart_destroy(struct Chart *chart)
{
    if (!chart) return;

    if (chart->tile) {
        tile_destroy(chart->tile);
        chart->tile = NULL;
    }

    if (chart->children) {
        for (int i = 0; i < NUM_CHILDREN; i++) {
            chart_destroy(chart_child(chart, i));
            chart->children[i] = NULL;
        }
        free(chart->children);
        chart->children = NULL;
    }

    free(chart);
}

struct Coordinate chart_coordinate(const struct Chart *chart)
{
    return chart->coordinate;
}

struct Tile *chart_tile(const struct Chart *chart)
{
    if (!chart) return NULL;
    return chart->tile;
}

void chart_clear_tile(struct Chart *chart)
{
    tile_destroy(chart_tile(chart));
    chart->tile = NULL;
}

void chart_set_tile(struct Chart *chart, struct Tile *tile)
{
    if (!chart || chart_tile(chart) || !tile) return;
    chart->tile = tile;
}

int chart_p(const struct Chart *chart)
{
    return coordinate_p(chart->coordinate);
}

int chart_q(const struct Chart *chart)
{
    return coordinate_q(chart->coordinate);
}

int chart_r(const struct Chart *chart)
{
    return coordinate_r(chart->coordinate);
}

enum TERRAIN chart_terrain(const struct Chart *chart)
{
    return tile_terrain(chart_tile(chart));
}


struct Atlas {
    struct Directory *directory;
    struct Chart *root;
    struct Chart *curr;
};

struct Atlas *atlas_create(void)
{
    struct Atlas *atlas = malloc(sizeof(struct Atlas));

    atlas->root = NULL;
    atlas->curr = NULL;
    atlas->directory = NULL;
    return atlas;
}

void atlas_initialise(struct Atlas *atlas)
{
    if (atlas->root) {
        return;
    }

    atlas->root = chart_create(coordinate_origin());
    atlas->curr = atlas->root;
}

void atlas_destroy(struct Atlas *atlas)
{
    if (!atlas) return;

    chart_destroy(atlas->root);
    directory_destroy(atlas->directory);

    atlas->root = NULL;
    atlas->curr = NULL;
    atlas->directory = NULL;
    free(atlas);
}

struct Directory *atlas_directory(const struct Atlas *atlas)
{
    return atlas->directory;
}

struct Chart *atlas_root(const struct Atlas *atlas)
{
    return atlas->root;
}

struct Chart *atlas_curr(const struct Atlas *atlas)
{
    return atlas->curr;
}

struct Coordinate atlas_coordinate(const struct Atlas *atlas)
{
    return chart_coordinate(atlas_curr(atlas));
}

struct Tile *atlas_tile(const struct Atlas *atlas)
{
    return chart_tile(atlas_curr(atlas));
}

enum TERRAIN atlas_terrain(const struct Atlas *atlas)
{
    return tile_terrain(atlas_tile(atlas));
}

void atlas_set_terrain(struct Atlas *atlas, enum TERRAIN t)
{
    tile_set_terrain(atlas_tile(atlas), t);
}

struct Chart *atlas_neighbour(const struct Atlas *atlas, enum DIRECTION d)
{
    struct Coordinate c = coordinate_shift(atlas_coordinate(atlas), d);
    return atlas_find(atlas, c);
}

void atlas_goto(struct Atlas *atlas, struct Coordinate c)
{
    struct Chart *chart = atlas_find(atlas, c);

    if (chart) atlas->curr = chart;
}

void atlas_step(struct Atlas *atlas, enum DIRECTION d)
{
    struct Chart *n = NULL;
    if ((n = atlas_neighbour(atlas, d))) atlas->curr = n;
}

struct Chart *atlas_find(const struct Atlas *atlas, struct Coordinate c)
{
    if (!atlas) return NULL;
    struct Coordinate r = chart_coordinate(atlas_root(atlas));
    if (!coordinate_related(r, c) || (coordinate_m(c) > coordinate_m(r))) return NULL;
    if (coordinate_equals(r, c)) return atlas_root(atlas);

    struct Coordinate p = coordinate_lift_by(c, 1);
    struct Chart *parent = atlas_find(atlas, p);
    if (!parent) return NULL;
    return chart_child(parent, coordinate_index(c));
}

void atlas_insert(struct Atlas *atlas, struct Chart *chart)
{
    if (!atlas || !chart) return;

    if (!atlas_root(atlas)) {
        atlas->root = chart;
        return;
    }

    struct Chart *root = atlas_root(atlas);
    struct Coordinate r = chart_coordinate(root);
    struct Coordinate c = chart_coordinate(chart);

    if (!coordinate_related(r, c)) {
        atlas->root = chart_create_ancestor(root, chart);
        atlas_insert(atlas, root);
        atlas_insert(atlas, chart);
        return;
    }

    struct Coordinate p = coordinate_lift_by(c, 1);
    struct Chart *parent = atlas_find(atlas, p);
    if (!parent) {
        parent = chart_create(p);
        atlas_insert(atlas, parent);
    }
    chart_set_child(parent, coordinate_index(c), chart);
}

void atlas_create_neighbours(struct Atlas *atlas)
{
    struct Coordinate n = coordinate_origin();
    struct Chart *neighbour = NULL;

    for (int i = 0; i < NUM_DIRECTIONS; i++) {
        n = coordinate_shift(chart_coordinate(atlas_curr(atlas)), i); 
        neighbour = atlas_find(atlas, n);
        if (!neighbour) {
            neighbour = chart_create(n);
            atlas_insert(atlas, neighbour);
        }
    }
}

void atlas_create_location(struct Atlas *atlas, enum LOCATION t)
{
    struct Location *new = location_create(atlas_coordinate(atlas), t);
    directory_insert(&(atlas->directory), new);
    tile_set_location(atlas_tile(atlas), new);
}

void atlas_add_location(struct Atlas *atlas, struct Location *location)
{
    directory_insert(&(atlas->directory), location);
    struct Coordinate c = atlas_coordinate(atlas);

    atlas_goto(atlas, location_coordinate(location));
    if (coordinate_equals(location_coordinate(location), atlas_coordinate(atlas))) {
        tile_set_location(atlas_tile(atlas), location);
    }
    atlas_goto(atlas, c);
}
