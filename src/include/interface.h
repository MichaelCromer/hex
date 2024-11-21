#ifndef INTERFACE_H
#define INTERFACE_H

#include <stdbool.h>

#include "enum.h"
#include "geometry.h"

struct UserInterface;

struct UserInterface *ui_create(void);
void ui_initialise(struct UserInterface *ui, struct Geometry *g);
void ui_destroy(struct UserInterface *ui);
void ui_toggle(struct UserInterface *ui, enum UI_PANEL p);
bool ui_show(struct UserInterface *ui, enum UI_PANEL p);
struct Panel *ui_panel(struct UserInterface *ui, enum UI_PANEL p);

#endif
