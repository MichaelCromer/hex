#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "hdr/atlas.h"
#include "hdr/coordinate.h"
#include "hdr/file.h"
#include "hdr/tile.h"

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

void write_coordinate(FILE *file, struct Coordinate c)
{
    if (!file) return;

    int32_t p = coordinate_p(c),
            q = coordinate_q(c),
            r = coordinate_r(c);
    uint32_t m = coordinate_m(c);

    fprintf(
        file,
        "%d%c%d%c%d%c%u%c",
        p, FILE_SEP_MIN,
        q, FILE_SEP_MIN,
        r, FILE_SEP_MIN,
        m, FILE_SEP_MIN
    );
}

void write_location(FILE *file, const struct Location *location)
{
    if (!file || !location) return;

    write_coordinate(file, location_coordinate(location));
    fputc(FILE_SEP_MAJ, file);
    fprintf(file, "%d", location_type(location));
}

void write_tile(FILE *file, const struct Tile *tile)
{
    if (!file || !tile) return;

    fprintf(file, "%u", tile_seed(tile));
    fputc(FILE_SEP_MED, file);
    fprintf(file, "%d", tile_terrain(tile));
    fputc(FILE_SEP_MED, file);

    /* road byte [..r6r5r4r3r2r1] */
    fprintf(file, "%d", tile_roads(tile));
    fputc(FILE_SEP_MED, file);

    /* river byte [..r6r5r4r3r2r1] */
    fprintf(file, "%d", tile_rivers(tile));
    fputc(FILE_SEP_MED, file);
}

void write_chart(FILE *file, const struct Chart *chart)
{
    if (!file || !chart) return;
    write_coordinate(file, chart_coordinate(chart));

    if (chart_tile(chart)) {
        fputc(FILE_SEP_MAJ, file);
        write_tile(file, chart_tile(chart));
    }
    fputc('\n', file);

    if (!chart_has_children(chart)) return;
    for (int i = 0; i < NUM_CHILDREN; i++) write_chart(file, chart_child(chart, i));
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
    if (!file || !atlas) return;

    fprintf(file, FILE_MARKER_ROOT "\n");
    write_chart(file, atlas_root(atlas));
    fprintf(file, FILE_MARKER_CURR "\n");
    write_coordinate(file, atlas_coordinate(atlas));
    fputc('\n', file);
    fprintf(file, FILE_MARKER_LOCN "\n");
    write_directory(file, atlas_directory(atlas));
}

void write_state(FILE *file)
{
    if (!file) return;

    write_atlas(file, state_atlas());
}

/*==========================================================
 *  READ
 *========================================================*/

struct Coordinate read_coordinate(char *str)
{
    if (!str) return (struct Coordinate) { 0 };

    char *str_p = str;
    char *str_q = strchrnul(str_p, FILE_SEP_MIN);
    *(str_q++) = '\0';
    char *str_r = strchrnul(str_q, FILE_SEP_MIN);
    *(str_r++) = '\0';
    char *str_m = strchrnul(str_r, FILE_SEP_MIN);
    *(str_m++) = '\0';
    char *str_end = strchrnul(str_m, FILE_SEP_MIN);
    *str_end = '\0';

    int32_t p = strtol(str_p, NULL, 10),
        q = strtol(str_q, NULL, 10), r = strtol(str_r, NULL, 10);
    uint32_t m = (uint32_t)strtol(str_m, NULL, 10);

    return coordinate(p, q, r, m);
}

/* expected format:
 *  [SEED];[TERRAIN];[ROADS];[RIVERS]
 */
struct Tile *read_tile(char *str)
{
    if (!str || strlen(str) == 0) return NULL;

    char *str_seed = str;
    char *str_terrain = strchrnul(str_seed, FILE_SEP_MED);
    *(str_terrain++) = '\0';
    char *str_roads = strchrnul(str_terrain, FILE_SEP_MED);
    *(str_roads++) = '\0';
    char *str_rivers = strchrnul(str_roads, FILE_SEP_MED);
    *(str_rivers++) = '\0';
    char *str_end = strchrnul(str_rivers, FILE_SEP_MED);
    *str_end = '\0';

    struct Tile *tile = tile_create();

    tile_set_seed(tile, strtol(str_seed, NULL, 10));
    tile_set_terrain(tile, (enum TERRAIN)strtol(str_terrain, NULL, 10));

    uint8_t roads = strtol(str_roads, NULL, 10);
    uint8_t rivers = strtol(str_rivers, NULL, 10);

    tile_set_roads(tile, roads);
    tile_set_rivers(tile, rivers);

    return tile;
}

struct Chart *read_chart(char *line)
{
    if (!line) return NULL;

    char *str_coordinate = line;
    char *str_tile = strchrnul(str_coordinate, FILE_SEP_MAJ);
    *(str_tile++) = '\0';

    struct Coordinate c = read_coordinate(str_coordinate);
    struct Chart *chart = chart_create(c);
    chart_clear_tile(chart);
    chart_set_tile(chart, read_tile(str_tile));
    return chart;
}


struct Location *read_location(char *line)
{
    char *str_coordinate = line;
    char *str_location = strchrnul(str_coordinate, FILE_SEP_MAJ);
    *(str_location++) = '\0';

    struct Coordinate c = read_coordinate(str_coordinate);
    enum LOCATION t = (enum LOCATION) strtol(str_location, NULL, 10);
    struct Location *location = location_create(c, t);
    return location;
}


struct Atlas *read_atlas(FILE *file)
{
    struct Atlas *atlas = atlas_create();

    char *buf = malloc(LINE_MAX);
    memset(buf, 0, LINE_MAX);

    char *line = NULL;
    size_t len = 0;
    ssize_t nread = 0;

    /* read in charts */
    while ((nread = getline(&line, &len, file)) > 0) {
        if (line[nread - 1] == '\n') line[nread - 1] = '\0';
        if (strcmp(line, FILE_MARKER_CURR) == 0) break;
        atlas_insert(atlas, read_chart(line));
    }

    /* set the current coordinate */
    while (fgets(buf, LINE_MAX, file)) {
        if (strcmp(buf, FILE_MARKER_LOCN "\n") == 0) break;
        atlas_goto(atlas, read_coordinate(buf));
    }

    /* read in locations */
    while (fgets(buf, LINE_MAX, file)) {
        if (strcmp(buf, FILE_MARKER_NULL "\n") == 0) break;
        atlas_add_location(atlas, read_location(buf));
    }

    free(buf);
    free(line);
    return atlas;
}


void read_state(FILE *file)
{
    if (!file) return;

    char *buf = malloc(LINE_MAX);
    memset(buf, 0, LINE_MAX);

    /* advance to 'chart' marker */
    while (fgets(buf, LINE_MAX, file)) {
        if (strcmp(buf, FILE_MARKER_ROOT "\n") == 0) break;
    }
    struct Atlas *atlas = read_atlas(file);

    state_clear_atlas();
    state_set_atlas(atlas);

    free(buf);
}
