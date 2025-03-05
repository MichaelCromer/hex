#include <stdlib.h>
#include <string.h>

#include "include/panel.h"

struct Panel {
    int r0, c0;
    int w, h;
    int len;
    char **lines;
};

struct Panel *panel_create(int r0, int c0, int len)
{
    struct Panel *p = malloc(sizeof(struct Panel));
    if (!p) {
        return NULL;
    }

    p->r0 = r0;
    p->c0 = c0;
    p->len = len;

    p->lines = malloc(len * sizeof(char *));
    if (!p->lines) {
        free(p);
        p = NULL;
        return NULL;
    }

    for (int i = 0; i < len; i++) {
        p->lines[i] = NULL;
    }

    p->w = 0;
    p->h = 0;
    return p;
}

void panel_destroy(struct Panel *p)
{
    if (!p) {
        return;
    }

    for (int i = 0; i < p->len; i++) {
        char *line = p->lines[i];
        if (line) {
            free(line);
        }
    }
    free(p->lines);
    p->lines = NULL;
    free(p);
    p = NULL;
}

int panel_row(struct Panel *p) { return p->r0; }
int panel_col(struct Panel *p) { return p->c0; }
int panel_width(struct Panel *p) { return p->w; }
int panel_height(struct Panel *p) { return p->h; }
int panel_len(struct Panel *p) { return p->len; }


char *panel_line(struct Panel *p, int i)
{
    if ((p != NULL) && (i >= 0) && (i < p->len)) {
        return p->lines[i];
    }
    return NULL;
}


void panel_add_line(struct Panel *p, int index, const char *line)
{
    if (index < 0 || index >= p->len) {
        return;
    }
    if (p->lines[index]) {
        return;
    }

    p->lines[index] = strdup(line);
    if (!p->lines[index]) {
        return;
    }

    int len = strlen(line);
    if ((len + 4) > p->w) {
        p->w = len + 4;
    }                           /* +4 to fit border and pad */
    if ((index + 5) > p->h) {
        p->h = index + 5;
    }                           /* +5 to fit border and pad, 0-indexed */

    return;
}


void panel_remove_line(struct Panel *p, int index)
{
    if (index < 0 || index >= p->len) {
        return;
    }
    if (!p->lines[index]) {
        return;
    }

    free(p->lines[index]);
    p->lines[index] = NULL;

    p->h = 0;
    p->w = 0;

    for (int i = 0; i < p->len; i++) {
        if (!p->lines[i]) {
            continue;
        }

        int len = strlen(p->lines[i]);
        if ((len + 4) > p->w) {
            p->w = len + 4;
        }
        if ((i + 5) > p->h) {
            p->h = i + 5;
        }
    }

    return;
}


void panel_set_rc(struct Panel *p, int r, int c)
{
    p->r0 = r;
    p->c0 = c;
}


void panel_centre(struct Panel *p, int r, int c)
{
    p->r0 = r - p->h/2;
    p->c0 = c - p->w/2;
}


struct Panel *panel_splash(void)
{
    struct Panel *splash = panel_create(0, 0, 6);
    panel_add_line(splash, 0, "WELCOME TO HEX                                 ");
    panel_add_line(splash, 1, "                                               ");
    panel_add_line(splash, 2, "  A ncurses hex map editor with modal editing. ");
    panel_add_line(splash, 3, "  Press <?> while editing for contextual help. ");
    panel_add_line(splash, 4, "                                               ");
    panel_add_line(splash, 5, "                              Close: <Enter>   ");

    return splash;
}

struct Panel *panel_tile_detail(void)
{
    struct Panel *chart_detail = panel_create(0, 0, 3);
    panel_add_line(chart_detail, 0, "Currently at: ");
    panel_add_line(chart_detail, 1, "    (p, q, r)");
    panel_add_line(chart_detail, 2, "    TERRAIN: NONE");

    return chart_detail;
}


struct Panel *panel_hint(void)
{
    struct Panel *hint = panel_create(0, 0, 12);
    panel_add_line(hint, 0, "MODES AND OPTIONS  ");
    panel_add_line(hint, 1, "                   ");
    panel_add_line(hint, 2, "  T/t   Terrain    ");
    panel_add_line(hint, 3, "  R/r   Roads      ");
    panel_add_line(hint, 4, "  Y/y   Rivers     ");
    panel_add_line(hint, 5, "  F/f   Locations  ");
    panel_add_line(hint, 6, "                   ");
    panel_add_line(hint, 7, "    :   Command    ");
    panel_add_line(hint, 8, "    ?   Help       ");

    return hint;
}


struct Panel *panel_navigate(void)
{
    struct Panel *navigate = panel_create(0, 0, 10);

    panel_add_line(navigate, 0, "MODE: NAVIGATION   ");
    panel_add_line(navigate, 1, "                   ");
    panel_add_line(navigate, 2, "      u   i        ");
    panel_add_line(navigate, 3, "       \\ /        ");
    panel_add_line(navigate, 4, "    h -   - k      ");
    panel_add_line(navigate, 5, "       / \\        ");
    panel_add_line(navigate, 6, "      n   m        ");
    panel_add_line(navigate, 7, "                   ");
    panel_add_line(navigate, 8, "  uihknm : 1 step  ");
    panel_add_line(navigate, 9, "  UIHKNM : 3 step  ");

    return navigate;
}


struct Panel *panel_terrain(void)
{
    struct Panel *terrain = panel_create(0, 0, 14);

    panel_add_line(terrain, 0,  "MODE: TERRAIN      ");
    panel_add_line(terrain, 1,  "                   ");
    panel_add_line(terrain, 2,  "  uihknm : move    ");
    panel_add_line(terrain, 3,  "  UIHKNM : drag    ");
    panel_add_line(terrain, 4,  "                   ");
    panel_add_line(terrain, 5,  "    q : water      ");
    panel_add_line(terrain, 6,  "    w : mountain   ");
    panel_add_line(terrain, 7,  "    e : hills      ");
    panel_add_line(terrain, 8,  "    a : plains     ");
    panel_add_line(terrain, 9,  "    s : forest     ");
    panel_add_line(terrain, 10, "    d : swamp      ");
    panel_add_line(terrain, 11, "    z : desert     ");
    panel_add_line(terrain, 12, "    x : jungle     ");
    panel_add_line(terrain, 13, "    c : tundra     ");

    return terrain;
}


struct Panel *panel_await_terrain(void)
{
    struct Panel *terrain = panel_create(0, 0, 11);

    panel_add_line(terrain, 0,  "MODE: (terrain)    ");
    panel_add_line(terrain, 1,  "                   ");
    panel_add_line(terrain, 2,  "    q : water      ");
    panel_add_line(terrain, 3,  "    w : mountain   ");
    panel_add_line(terrain, 4,  "    e : hills      ");
    panel_add_line(terrain, 5,  "    a : plains     ");
    panel_add_line(terrain, 6,  "    s : forest     ");
    panel_add_line(terrain, 7,  "    d : swamp      ");
    panel_add_line(terrain, 8,  "    z : desert     ");
    panel_add_line(terrain, 9,  "    x : jungle     ");
    panel_add_line(terrain, 10, "    c : tundra     ");

    return terrain;
}


struct Panel *panel_road(void)
{
    struct Panel *road = panel_create(0, 0, 9);

    panel_add_line(road, 0,  "MODE: ROAD         ");
    panel_add_line(road, 1,  "                   ");
    panel_add_line(road, 2,  "  uihknm : move    ");
    panel_add_line(road, 3,  "  UIHKNM : drag    ");
    panel_add_line(road, 4,  "                   ");
    panel_add_line(road, 5,  "  Drag over once   ");
    panel_add_line(road, 6,  "  to place road,   ");
    panel_add_line(road, 7,  "  drag a second    ");
    panel_add_line(road, 8,  "  time to remove   ");

    return road;
}


struct Panel *panel_await_road(void)
{
    struct Panel *road = panel_create(0, 0, 9);

    panel_add_line(road, 0,  "MODE: (road)       ");
    panel_add_line(road, 1,  "                   ");
    panel_add_line(road, 2,  "  uihknm : paint   ");

    return road;
}


struct Panel *panel_river(void)
{
    struct Panel *river = panel_create(0, 0, 12);

    panel_add_line(river, 0,  "MODE: RIVER        ");
    panel_add_line(river, 1,  "                   ");
    panel_add_line(river, 2,  "  uihknm : move    ");
    panel_add_line(river, 3,  "  UIHKNM : drag    ");
    panel_add_line(river, 4,  "                   ");
    panel_add_line(river, 5,  "  Drag over once   ");
    panel_add_line(river, 6,  "  to place road,   ");
    panel_add_line(river, 7,  "  drag a second    ");
    panel_add_line(river, 8,  "  time to remove   ");

    return river;
}
