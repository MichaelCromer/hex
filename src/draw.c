#include <stdlib.h>
#include <ncurses.h>
#include <math.h>

#include "draw.h"
#include "queue.h"


/*
 *      GEOMETRY
 */

struct Geometry {
    float scale;
    float aspect;
    float slope;
    int hex_w, hex_h;
    int cols, rows;
    int rmid, cmid;
};


/*  Geometry lifetime pipeline */

void geometry_update(struct Geometry *g)
{
    g->hex_w = round(g->scale * ROOT3 / 2);
    g->hex_h = round(g->scale * g->aspect / 2);
    g->slope = ROOT3_INV * g->aspect;
    g->rmid = g->rows / 2;
    g->cmid = g->cols / 2;
}

struct Geometry *geometry_create(float scale, float aspect, int cols, int rows)
{
    struct Geometry *g = malloc(sizeof(struct Geometry));

    g->scale = scale;
    g->aspect = aspect;
    g->cols = cols;
    g->rows = rows;

    geometry_update(g);

    return g;
}

void geometry_destroy(struct Geometry *g)
{
    free(g);
    g = NULL;
}


/* Geometry getters */

float geometry_scale(struct Geometry *g)
{
    return g->scale;
}

float geometry_aspect(struct Geometry *g)
{
    return g->aspect;
}

float geometry_slope(struct Geometry *g)
{
    return g->slope;
}

int geometry_hex_h(struct Geometry *g)
{
    return g->hex_h;
}

int geometry_hex_w(struct Geometry *g)
{
    return g->hex_w;
}

int geometry_cols(struct Geometry *g)
{
    return g->cols;
}

int geometry_rows(struct Geometry *g)
{
    return g->rows;
}

int geometry_rmid(struct Geometry *g)
{
    return g->rmid;
}

int geometry_cmid(struct Geometry *g)
{
    return g->cmid;
}


/* Geometry setters */

void geometry_set_scale(struct Geometry *g, float scale)
{
    g->scale = scale;
    geometry_update(g);
}


void geometry_set_aspect(struct Geometry *g, float aspect)
{
    g->aspect = aspect;
    geometry_update(g);
}


/*
 *      DRAW 01 - Basic shapes
 */

int draw_border(int r0, int c0, int w, int h)
{
    mvhline(r0,     c0,     ACS_HLINE, w-1);
    mvvline(r0,     c0,     ACS_VLINE, h-1);
    mvvline(r0,     c0+w-1, ACS_VLINE, h-1);
    mvhline(r0+h-1, c0,     ACS_HLINE, w-1);

    mvaddch(r0,     c0,     ACS_PLUS);
    mvaddch(r0,     c0+w-1, ACS_PLUS);
    mvaddch(r0+h-1, c0,     ACS_PLUS);
    mvaddch(r0+h-1, c0+w-1, ACS_PLUS);

    return 0;
}


int draw_rectangle(int r0, int c0, int w, int h, char bg)
{
    for (int r=0; r<h; r++) {
        mvhline(r0+r, c0, bg, w);
    }

    return 0;
}


int draw_box(int r0, int c0, int w, int h, char bg)
{
    draw_rectangle(r0, c0, w, h, bg);
    draw_border(r0, c0, w, h);

    return 0;
}


/*
 *      DRAW 02 - Panels
 */

int draw_panel(struct Panel *p)
{
    int r = panel_row(p), c = panel_col(p);
    int w = panel_width(p), h = panel_height(p);
    draw_box(r, c, w, h, ' ');

    for (int i=0; i<panel_len(p); i++) {
        char *line = panel_line(p, i);
        if (!line) { continue; }
        mvprintw(r+2+i, c+2, "%s", line);
    }
    return 0;
}


int clear_panel(struct Panel *p)
{
    int r = panel_row(p), c = panel_col(p);
    int w = panel_width(p), h = panel_height(p);
    draw_rectangle(r, c, w, h, ' ');

    return 0;
}

/*
 *      DRAW 03 - Hexes and terrain
 */

char get_terrainchr(enum TERRAIN t)
{
    switch (t) {
        case WATER:
            return '~';
        case MOUNTAINS:
            return '^';
        case PLAINS:
            return ';';
        case HILLS:
            return 'n';
        case FOREST:
            return 'T';
        case DESERT:
            return '*';
        case JUNGLE:
            return '#';
        case SWAMP:
            return 'j';
        default:
            break;
    }
    return '?';
}


void draw_reticule(struct Geometry *g)
{
    char ch = '#';
    int dh = 0;

    float slope = geometry_slope(g);
    int rmid = geometry_rmid(g), cmid = geometry_cmid(g);
    int hex_w = geometry_hex_w(g), hex_h = geometry_hex_h(g);

    mvvline(rmid-hex_h, cmid-hex_w, ch, 2*hex_h);
    mvvline(rmid-hex_h, cmid+hex_w, ch, 2*hex_h);

    mvvline(rmid-hex_h, cmid-hex_w+1, ch, 2*hex_h);
    mvvline(rmid-hex_h, cmid+hex_w-1, ch, 2*hex_h);

    for (int col = -hex_w; col <= hex_w; col++) {
        dh = (col < 0) ? round((hex_w+col)*slope) : round((hex_w-col)*slope);
        mvaddch(rmid - (hex_h + dh), cmid + col, ch);
        mvaddch(rmid + (hex_h + dh), cmid + col, ch);
    }

    return;
}


int draw_hex(struct Hex *hex, int row0, int col0, int w_half, int h_half, float slope)
{
    char ch = get_terrainchr(hex_get_terrain(hex));
    int dh = 0;

    for (int col = -w_half; col <= w_half; col++) {
        dh = (col < 0) ? round((w_half+col)*slope) : round((w_half-col)*slope);

        for (int row = -(h_half + dh); row <= (h_half + dh); row++) {
            mvaddch(row0 + row, col0 + col, ch);
        }
    }
    return 0;
}


void draw_map(struct Geometry *g, struct Hex *origin)
{
    /* for queuing tiles to be drawn */
    struct Queue *open = queue_create(origin, 0);
    struct Queue *closed = NULL;
    struct Hex *curr = NULL;
    struct Hex *nbr = NULL;

    float scale = geometry_scale(g),
          aspect = geometry_aspect(g),
          slope = geometry_slope(g);
    int rows = geometry_rows(g), cols = geometry_cols(g);
    int rmid = geometry_rmid(g), cmid = geometry_cmid(g);
    int hex_h = geometry_hex_h(g), hex_w = geometry_hex_w(g);

    /* tile-location tracking & calculation */
    int r, c;
    float dc, dr;

    /* check which tiles should be drawn */
    while (open) {
        /* pop the top */
        curr = queue_hex(open);
        queue_remove(&open, curr);
        queue_add(&closed, curr, 0);

        /* add its neighbours tho */
        for (int i=0; i<6; i++) {
            nbr = hex_neighbour(curr, i);
            if ((queue_find(closed, nbr) != NULL) || (queue_find(open, nbr) != NULL)) {
                continue;
            }
            queue_add(&open, nbr, 0);
        }

        /* calc the geometry */
        dc = scale * (hex_u(curr) - hex_u(origin));
        dr = aspect * scale * (hex_v(curr) - hex_v(origin));
        c = round(cmid + dc);
        r = round(rmid + dr);
        if (r < -hex_h || r > (rows+hex_h) || c < -hex_w || c > (cols+hex_w)) {
            continue;
        }
        draw_hex(curr, r, c, hex_w, hex_h, slope);
    }

    queue_destroy(open);
    queue_destroy(closed);
    return;
}


/*
 *      DRAW 04 - Core stuff
 */

int draw_screen(struct Geometry *g, struct Hex *map)
{
    erase();

    draw_map(g, map);
    draw_border(0, 0, geometry_cols(g), geometry_rows(g));
    draw_reticule(g);

    refresh();
    return 0;
}
