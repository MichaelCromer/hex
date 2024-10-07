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
    if (!p) return NULL;

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
    if (p) {
        for (int i=0; i<p->len; i++) {
            free(p->lines[i]);
        }
        free(p->lines);
        p->lines = NULL;
        free(p);
        p = NULL;
    }
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
