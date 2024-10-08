#ifndef PANEL_H
#define PANEL_H


struct Panel;

struct Panel *panel_create(int r0, int c0, int len);
void panel_destroy(struct Panel *p);
int panel_add_line(struct Panel *p, const char *line, int index);
int panel_remove_line(struct Panel *p, int index);
int panel_set_rc(struct Panel *p, int r, int c);
int panel_row(struct Panel *p);
int panel_col(struct Panel *p);
int panel_width(struct Panel *p);
int panel_height(struct Panel *p);
int panel_len(struct Panel *p);
char *panel_line(struct Panel *p, int i);

#endif
