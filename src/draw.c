#include <ncurses.h>


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


int draw_panel(struct Panel *p)
{
    draw_box(p->r0, p->c0, p->w, p->h, ' ');

    for (int i=0; i<p->len; i++) {
        if (!p->lines[i]) { continue; }
        mvprintw(p->r0+2+i, p->c0+2, "%s", p->lines[i]);
    }
    return 0;
}


int clear_panel(struct Panel *p)
{
    draw_rectangle(p->r0, p->c0, p->w, p->h, ' ');

    return 0;
}


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


int draw_hex(int row0, int col0, struct Hex *hex)
{
    char ch = get_terrainchr(hex->t);
    int dh = 0;

    for (int col = -_hex_w; col <= _hex_w; col++) {
        dh = (col < 0) ? round((_hex_w+col)*_DH_DW) : round((_hex_w-col)*_DH_DW);
        for (int row = -(_hex_h + dh); row <= (_hex_h + dh); row++) {
            mvaddch(row0 + row, col0 + col, ch);
        }
    }
    return 0;
}


int draw_screen(void)
{
    draw_border(0, 0, _cols, _rows);
    draw_hex(_rmid, _cmid, _h);

    if (SPLASH) {
        draw_panel(_splash);
    }
    if (TERRAIN_SELECTOR) {
        draw_panel(_terrain_selector);
    }

    refresh();
    return 0;
}


