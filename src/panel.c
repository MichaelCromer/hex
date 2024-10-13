#include <stdlib.h>
#include <string.h>

#include "panel.h"

struct Panel {
    int r0, c0;
    int w, h;
    int len;
    char **lines;
};


struct Panel *panel_create(int r0, int c0, int len)
{
    struct Panel *p = malloc(sizeof(struct Panel));
    if (!p) { return NULL; }

    p->r0 = r0;
    p->c0 = c0;
    p->len = len;

    p->lines = malloc(len * sizeof(char *));
    if (!p->lines) {
        free(p);
        return NULL;
    }

    for (int i=0; i<len; i++) {
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

    for (int i=0; i<p->len; i++) {
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


int panel_row(struct Panel *p)
{
    return p->r0;
}


int panel_col(struct Panel *p)
{
    return p->c0;
}


int panel_width(struct Panel *p)
{
    return p->w;
}


int panel_height(struct Panel *p)
{
    return p->h;
}


int panel_len(struct Panel *p)
{
    return p->len;
}


char *panel_line(struct Panel *p, int i)
{
    if ( (p != NULL) && (i >= 0) && (i < p->len) ) {
        return p->lines[i];
    }
    return NULL;
}


int panel_add_line(struct Panel *p, const char *line, int index)
{
    if (index < 0 || index >= p->len) { return -1; }
    if (p->lines[index]) { free(p->lines[index]); }

    p->lines[index] = strdup(line);
    if (!p->lines[index]) { return -1; }

    int len = strlen(line);
    if ((len+4) > p->w) { p->w = len+4; } /* +4 to fit border and pad */
    if ((index+5) > p->h) { p->h = index+5; } /* +5 to fit border and pad, 0-indexed */

    return 0;
}


int panel_remove_line(struct Panel *p, int index)
{
    if (index < 0 || index >= p->len) { return -1; }
    if (!p->lines[index]) { return 0; }

    free(p->lines[index]);
    p->h = 0;
    p->w = 0;

    for (int i=0; i<p->len; i++) {
        if (!p->lines[i]) { continue; }

        int len = strlen(p->lines[i]);
        if ((len+4) > p->w) { p->w = len+4; }
        if ((i+5) > p->h) { p->h = i+5; }
    }

    return 0;
}


int panel_set_rc(struct Panel *p, int r, int c)
{
    p->r0 = r;
    p->c0 = c;

    return 0;
}


void panel_centre(struct Panel *p, int r, int c)
{
    p->r0 = r - p->h/2;
    p->c0 = c - p->w/2;
}


struct Panel *panel_splash(void)
{
    struct Panel *splash = panel_create(0, 0, 5);
    panel_add_line(splash, "Welcome to hex",                           0);
    panel_add_line(splash, "Use u,i,h,l,n,m to navigate tiles",        2);
    panel_add_line(splash, "Use j to interact with the current tile",  3);
    panel_add_line(splash, "Shift-q to exit.",                         4);

    return splash;
}


struct Panel *panel_terrain_selector(void)//int rmid, int cmid)
{
    struct Panel *terrain_selector = panel_create(9, 2, 5);
    panel_add_line(terrain_selector, "Select Terrain:", 0);
    panel_add_line(terrain_selector, "1. Ocean     2. Mountain 3. Plains", 2);
    panel_add_line(terrain_selector, "4. Hills     5. Forest   6. Desert", 3);
    panel_add_line(terrain_selector, "7. Jungle    8. Swamp    q. Close ", 4);
    //int r0 = rmid - (panel_height(terrain_selector) / 2);
    //int c0 = cmid - (panel_width(terrain_selector) / 2);
    //panel_set_rc(terrain_selector, r0, c0);

    return terrain_selector;
}


struct Panel *panel_hex_detail(void)
{
    struct Panel *hex_detail = panel_create(2, 2, 3);
    panel_add_line(hex_detail, "Currently at: ",    0);
    panel_add_line(hex_detail, "    (p, q, r)",     1);
    panel_add_line(hex_detail, "    TERRAIN: NONE", 2);

    return hex_detail;
}
