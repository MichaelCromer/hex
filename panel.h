#ifndef PANEL_H
#define PANEL_H


struct Panel {
    int w, h;
    int len;
    char **lines;
};


struct Panel *panel_create(int len)
{
    struct Panel *p = malloc(sizeof(struct Panel));
    if (!p) return NULL;

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
        free(p);
    }
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


#endif
