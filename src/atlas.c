#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "include/atlas.h"
#include "include/tile.h"

struct Chart {
    struct Coordinate *coordinate;
    struct Tile *tile;
    struct Chart **children;
};

struct Chart *chart_create(const struct Coordinate *c)
{
    struct Chart *chart = malloc(sizeof(struct Chart));

    chart->coordinate = coordinate_duplicate(c);
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
    struct Coordinate *a = coordinate_create_ancestor(chart_coordinate(chart1),
                                                      chart_coordinate(chart2));
    struct Chart *ancestor = chart_create(a);
    coordinate_destroy(a);
    return ancestor;
}

struct Chart *chart_create_origin(void)
{
    return chart_create(coordinate_origin());
}

struct Chart **chart_children(const struct Chart *chart)
{
    return chart->children;
}

struct Chart *chart_child(const struct Chart *chart, enum CHILDREN c)
{
    if (!chart || !chart->children) {
        return NULL;
    }

    return chart->children[c];
}

void chart_set_child(struct Chart *chart, enum CHILDREN c, struct Chart *child)
{
    if (!chart || chart_child(chart, c)) {
        return;
    }

    chart->children[c] = child;
}

void chart_destroy(struct Chart *chart)
{
    if (!chart) {
        return;
    }

    coordinate_destroy(chart->coordinate);
    chart->coordinate = NULL;

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

struct Coordinate *chart_coordinate(const struct Chart *chart)
{
    return chart->coordinate;
}

struct Tile *chart_tile(const struct Chart *chart)
{
    if (!chart) {
        return NULL;
    }
    return chart->tile;
}

void chart_clear_tile(struct Chart *chart)
{
    tile_destroy(chart_tile(chart));
    chart->tile = NULL;
}

void chart_set_tile(struct Chart *chart, struct Tile *tile)
{
    if (!chart || chart_tile(chart) || !tile) {
        return;
    }

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
    struct Coordinate *screen_L, *screen_R, *screen_T, *screen_B, *viewpoint;
};

struct Atlas *atlas_create(void)
{
    struct Atlas *atlas = malloc(sizeof(struct Atlas));

    atlas->root = NULL;
    atlas->curr = NULL;
    atlas->directory = NULL;
    atlas->screen_L = NULL;
    atlas->screen_R = NULL;
    atlas->screen_T = NULL;
    atlas->screen_B = NULL;
    atlas->viewpoint = NULL;

    return atlas;
}

void atlas_initialise(struct Atlas *atlas, struct Geometry *g)
{
    if (atlas->root) {
        return;
    }

    atlas->root = chart_create_origin();
    atlas->curr = atlas->root;

    atlas_recalculate_screen(atlas, g);
}

void atlas_destroy(struct Atlas *atlas)
{
    if (!atlas) {
        return;
    }

    chart_destroy(atlas->root);
    directory_destroy(atlas->directory);
    coordinate_destroy(atlas->screen_L);
    coordinate_destroy(atlas->screen_R);
    coordinate_destroy(atlas->screen_T);
    coordinate_destroy(atlas->screen_B);
    coordinate_destroy(atlas->viewpoint);

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

struct Coordinate *atlas_coordinate(const struct Atlas *atlas)
{
    return chart_coordinate(atlas_curr(atlas));
}

struct Tile *atlas_tile(const struct Atlas *atlas)
{
    return chart_tile(atlas_curr(atlas));
}

enum TERRAIN atlas_terrain(const struct Atlas *atlas)
{
    return tile_terrain(chart_tile(atlas_curr(atlas)));
}

void atlas_set_terrain(struct Atlas *atlas, enum TERRAIN t)
{
    tile_set_terrain(chart_tile(atlas_curr(atlas)), t);
}

struct Chart *atlas_neighbour(const struct Atlas *atlas, enum DIRECTION d)
{
    struct Coordinate *c = coordinate_duplicate(atlas_coordinate(atlas));
    coordinate_shift(c, d);
    struct Chart *neighbour = atlas_find(atlas, c);
    coordinate_destroy(c);
    return neighbour;
}

void atlas_goto(struct Atlas *atlas, struct Coordinate *c)
{
    struct Chart *chart = atlas_find(atlas, c);

    if (c) {
        atlas->curr = chart;
    }
}

void atlas_step(struct Atlas *atlas, enum DIRECTION d)
{
    struct Chart *n = NULL;
    if ((n = atlas_neighbour(atlas, d))) {
        atlas->curr = n;
        coordinate_shift(atlas->screen_L, d);
        coordinate_shift(atlas->screen_R, d);
        coordinate_shift(atlas->screen_T, d);
        coordinate_shift(atlas->screen_B, d);
        atlas_recalculate_viewpoint(atlas);
    }
}

struct Chart *atlas_find(const struct Atlas *atlas, const struct Coordinate *c)
{
    if (!atlas || !c) {
        return NULL;
    }

    struct Coordinate *r = chart_coordinate(atlas_root(atlas));

    if (!coordinate_related(r, c) || (coordinate_m(c) > coordinate_m(r))) {
        return NULL;
    }

    if (coordinate_equals(r, c)) {
        return atlas_root(atlas);
    }

    struct Coordinate *p = coordinate_create_parent(c);
    struct Chart *parent = atlas_find(atlas, p);

    if (!parent) {
        return NULL;
    }

    return chart_child(parent, coordinate_index(c));
}

void atlas_insert(struct Atlas *atlas, struct Chart *chart)
{
    if (!atlas || !chart) {
        return;
    }

    if (!atlas_root(atlas)) {
        atlas->root = chart;
        return;
    }

    struct Chart *root = atlas_root(atlas);
    struct Coordinate *r = chart_coordinate(root);
    struct Coordinate *n = chart_coordinate(chart);

    if (!coordinate_related(r, n)) {
        atlas->root = chart_create_ancestor(root, chart);
        atlas_insert(atlas, root);
        atlas_insert(atlas, chart);
        return;
    }

    struct Coordinate *p = coordinate_create_parent(n);
    struct Chart *parent = atlas_find(atlas, p);
    if (!parent) {
        parent = chart_create(p);
        atlas_insert(atlas, parent);
    }
    chart_set_child(parent, coordinate_index(n), chart);
}

void atlas_create_neighbours(struct Atlas *atlas)
{
    struct Coordinate *n = coordinate_create_origin();
    struct Chart *neighbour = NULL;

    for (int i = 0; i < NUM_DIRECTIONS; i++) {
        coordinate_add(chart_coordinate(atlas_curr(atlas)), coordinate_delta(i), n);
        neighbour = atlas_find(atlas, n);
        if (!neighbour) {
            neighbour = chart_create(n);
            atlas_insert(atlas, neighbour);
        }
    }

    coordinate_destroy(n);
    return;
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
    struct Coordinate *c = atlas_coordinate(atlas); /* store the old spot */
    atlas_goto(atlas, location_coordinate(location));
    if (coordinate_equals(location_coordinate(location), atlas_coordinate(atlas))) {
        tile_set_location(atlas_tile(atlas), location);
    }
    atlas_goto(atlas, c); /* go back to the old spot */
}

struct Coordinate *atlas_viewpoint(struct Atlas *atlas)
{
    return atlas->viewpoint;
}

void atlas_recalculate_viewpoint(struct Atlas *atlas)
{
    struct Coordinate *tmp1 =
        coordinate_create_ancestor(atlas->screen_L, atlas->screen_R);
    struct Coordinate *tmp2 =
        coordinate_create_ancestor(atlas->screen_T, atlas->screen_B);

    coordinate_common_ancestor(tmp1, tmp2, atlas->viewpoint);

    coordinate_destroy(tmp1);
    coordinate_destroy(tmp2);
}

void atlas_recalculate_screen(struct Atlas *atlas, struct Geometry *g)
{
    int n_h = geometry_tile_nh(g), n_w = geometry_tile_nw(g);

    coordinate_destroy(atlas->screen_L);
    coordinate_destroy(atlas->screen_R);
    coordinate_destroy(atlas->screen_T);
    coordinate_destroy(atlas->screen_B);
    coordinate_destroy(atlas->viewpoint);

    atlas->screen_L = coordinate_duplicate(atlas_coordinate(atlas));
    atlas->screen_R = coordinate_duplicate(atlas_coordinate(atlas));
    atlas->screen_T = coordinate_duplicate(atlas_coordinate(atlas));
    atlas->screen_B = coordinate_duplicate(atlas_coordinate(atlas));
    atlas->viewpoint = coordinate_create_origin();

    coordinate_nshift(atlas->screen_L, DIRECTION_WW, n_w / 2);
    coordinate_nshift(atlas->screen_R, DIRECTION_EE, n_w / 2);
    coordinate_nshift(atlas->screen_T, DIRECTION_NW, n_h / 4);
    coordinate_nshift(atlas->screen_T, DIRECTION_NE, n_h / 4);
    coordinate_nshift(atlas->screen_B, DIRECTION_SW, n_h / 4);
    coordinate_nshift(atlas->screen_B, DIRECTION_SE, n_h / 4);

    atlas_recalculate_viewpoint(atlas);
}
