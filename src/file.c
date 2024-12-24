#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/atlas.h"
#include "include/coordinate.h"
#include "include/file.h"
#include "include/tile.h"

#define FILE_MARKER_ROOT "===ROOT==="
#define FILE_MARKER_CURR "===CURR==="
#define FILE_MARKER_LOCN "===LOCN==="
#define FILE_MARKER_NULL "===NULL==="

#define FILE_SEP_MAJ ':'
#define FILE_SEP_MED ';'
#define FILE_SEP_MIN ','

/*==========================================================
 *  WRITE
 *========================================================*/

void write_coordinate(FILE *file, const struct Coordinate *c)
{
    if (!file || !c) {
        return;
    }

    int p = coordinate_p(c),
        q = coordinate_q(c), r = coordinate_r(c), m = coordinate_m(c);

    fprintf(file, "%d%c%d%c%d%c%u%c", p, FILE_SEP_MIN, q, FILE_SEP_MIN, r, FILE_SEP_MIN,
            m, FILE_SEP_MIN);
}

void write_location(FILE *file, const struct Location *location)
{
    if (!file || !location) {
        return;
    }

    write_coordinate(file, location_coordinate(location));
    fputc(FILE_SEP_MAJ, file);
    fprintf(file, "%d", location_type(location));
}

void write_tile(FILE *file, const struct Tile *tile)
{
    if (!file || !tile) {
        return;
    }

    fprintf(file, "%u", tile_seed(tile));
    fputc(FILE_SEP_MED, file);
    fprintf(file, "%d", tile_terrain(tile));
    fputc(FILE_SEP_MED, file);

    for (int i = 0; i < NUM_DIRECTIONS; i++) {
        fprintf(file, "%d", tile_road(tile, i));
        fputc(FILE_SEP_MIN, file);
    }

    fputc(FILE_SEP_MED, file);

    for (int i = 0; i < NUM_DIRECTIONS; i++) {
        fprintf(file, "%d", tile_river(tile, i));
        fputc(FILE_SEP_MIN, file);
    }
}

void write_chart(FILE *file, const struct Chart *chart)
{
    if (!file || !chart) {
        return;
    }

    write_coordinate(file, chart_coordinate(chart));
    if (chart_tile(chart)) {
        fputc(FILE_SEP_MAJ, file);
        write_tile(file, chart_tile(chart));
    }
    fputc('\n', file);

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
        fputc('\n', file);
        curr = directory_next(curr);
    }
}

void write_atlas(FILE *file, const struct Atlas *atlas)
{
    if (!file || !atlas) {
        return;
    }

    fprintf(file, FILE_MARKER_ROOT "\n");
    write_chart(file, atlas_root(atlas));
    fprintf(file, FILE_MARKER_CURR "\n");
    write_coordinate(file, atlas_coordinate(atlas));
    fputc('\n', file);
    fprintf(file, FILE_MARKER_LOCN "\n");
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

/*==========================================================
 *  READ
 *========================================================*/

struct Coordinate *read_coordinate(char *str)
{
    if (!str) {
        return NULL;
    }

    char delim[2] = { FILE_SEP_MIN, '\0' };
    char *str_p = strtok(str, delim);
    char *str_q = strtok(NULL, delim);
    char *str_r = strtok(NULL, delim);
    char *str_m = strtok(NULL, delim);

    int p = strtol(str_p, NULL, 10),
        q = strtol(str_q, NULL, 10), r = strtol(str_r, NULL, 10);
    unsigned int m = (unsigned int)strtol(str_m, NULL, 10);

    return coordinate_create(p, q, r, m);
}

/* expected format:
 *  [SEED];[TERRAIN];[ROAD1],[ROAD2],...,[ROAD6],;[RIVER1],[RIVER2],...,[RIVER6],
 */
struct Tile *read_tile(char *str)
{
    if (!str) {
        return NULL;
    }

    char delim_med[2] = { FILE_SEP_MED, '\0' };
    char delim_min[2] = { FILE_SEP_MIN, '\0' };
    char *str_seed = strtok(str, delim_med);
    char *str_terrain = strtok(NULL, delim_med);
    char *str_roads = strtok(NULL, delim_med);
    char *str_rivers = strtok(NULL, delim_med);

    struct Tile *tile = tile_create();

    tile_set_seed(tile, strtol(str_seed, NULL, 10));
    tile_set_terrain(tile, (enum TERRAIN)strtol(str_terrain, NULL, 10));

    int i = 0;
    char *str_road = strtok(str_roads, delim_min);
    do {
        tile_set_road(tile, (enum DIRECTION)i, (bool)strtol(str_road, NULL, 10));
        str_road = strtok(NULL, delim_min);
        i++;
    } while (i < NUM_DIRECTIONS);

    int j = 0;
    char *str_river = strtok(str_rivers, delim_min);
    do {
        tile_set_river(tile, (enum DIRECTION)i, (bool)strtol(str_river, NULL, 10));
        str_river = strtok(NULL, delim_min);
        j++;
    } while (j < NUM_DIRECTIONS);

    return tile;
}

struct Chart *read_chart(char *line)
{
    if (!line) {
        return NULL;
    }

    char delim[2] = { FILE_SEP_MAJ, '\0' };

    char *str_coordinate = strtok(line, delim);
    char *str_tile = strtok(NULL, delim);

    struct Coordinate *c = read_coordinate(str_coordinate);

    struct Chart *chart = chart_create(c);
    chart_clear_tile(chart);
    chart_set_tile(chart, read_tile(str_tile));
    coordinate_destroy(c);

    return chart;
}


struct Location *read_location(char *line)
{
    char delim[2] = { FILE_SEP_MAJ, '\0' };

    char *str_coordinate = strtok(line, delim);
    char *str_location = strtok(NULL, delim);

    struct Coordinate *c = read_coordinate(str_coordinate);
    enum LOCATION t = (enum LOCATION)strtol(str_location, NULL, 10);

    struct Location *location = location_create(c, t);

    coordinate_destroy(c);
    return location;
}


struct Atlas *read_atlas(FILE *file)
{
    struct Atlas *atlas = atlas_create();

    char *buf = malloc(LINE_MAX);
    memset(buf, 0, LINE_MAX);

    /* read in charts */
    while (fgets(buf, LINE_MAX, file)) {
        if (strcmp(buf, FILE_MARKER_CURR "\n") == 0) {
            break;
        }
        atlas_insert(atlas, read_chart(buf));
    }

    /* set the current coordinate */
    while (fgets(buf, LINE_MAX, file)) {
        if (strcmp(buf, FILE_MARKER_LOCN "\n") == 0) {
            break;
        }
        atlas_goto(atlas, read_coordinate(buf));
    }

    /* read in locations */
    while (fgets(buf, LINE_MAX, file)) {
        if (strcmp(buf, FILE_MARKER_NULL "\n") == 0) {
            break;
        }
        atlas_add_location(atlas, read_location(buf));
    }

    free(buf);
    return atlas;
}


void read_state(FILE *file, struct State *state)
{
    if (!file) {
        return;
    }

    char *buf = malloc(LINE_MAX);
    memset(buf, 0, LINE_MAX);

    /* advance to 'chart' marker */
    while (fgets(buf, LINE_MAX, file)) {
        if (strcmp(buf, FILE_MARKER_ROOT "\n") == 0) {
            break;
        }
    }
    struct Atlas *atlas = read_atlas(file);

    atlas_recalculate_screen(atlas, state_geometry(state));
    state_clear_atlas(state);
    state_set_atlas(state, atlas);

    free(buf);
}
