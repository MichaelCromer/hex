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

#endif
