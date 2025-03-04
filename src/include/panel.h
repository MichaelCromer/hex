#ifndef PANEL_H
#define PANEL_H

struct Panel;

void panel_destroy(struct Panel *p);
void panel_add_line(struct Panel *p, int index, const char *line);
void panel_remove_line(struct Panel *p, int index);
void panel_centre(struct Panel *p, int r, int c);
void panel_set_rc(struct Panel *p, int r, int c);
int panel_row(struct Panel *p);
int panel_col(struct Panel *p);
int panel_width(struct Panel *p);
int panel_height(struct Panel *p);
int panel_len(struct Panel *p);
char *panel_line(struct Panel *p, int i);

struct Panel *panel_splash(void);
struct Panel *panel_tile_detail(void);
struct Panel *panel_hint(void);
struct Panel *panel_navigate(void);

#endif
