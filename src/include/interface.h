#ifndef INTERFACE_H
#define INTERFACE_H

#include <stdbool.h>

#include "enum.h"
#include "state.h"

struct UserInterface;

void ui_initialise(void);
void ui_toggle_show(enum UI_PANEL p);
bool ui_is_show(enum UI_PANEL p);
struct Panel *ui_panel(enum UI_PANEL p);
void ui_update(struct State *state);

int panel_row(struct Panel *p);
int panel_col(struct Panel *p);
unsigned int panel_width(struct Panel *p);
unsigned int panel_height(struct Panel *p);
size_t panel_len(struct Panel *p);
char *panel_line(struct Panel *p, size_t i);

#endif
