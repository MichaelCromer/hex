#ifndef INTERFACE_H
#define INTERFACE_H

#include <stdbool.h>

#define UI_NUM_PANELS 3

enum UI_PANEL {
   SPLASH,
   TERRAIN_SELECTOR,
   HEX_DETAIL
};


struct UserInterface;

struct UserInterface *ui_create(void);
struct UserInterface *ui_initialise(void);
void ui_destroy(struct UserInterface *ui);
void ui_toggle(struct UserInterface *ui, enum UI_PANEL p);
bool ui_show(struct UserInterface *ui, enum UI_PANEL p);
struct Panel *ui_panel(struct UserInterface *ui, enum UI_PANEL p);

#endif
