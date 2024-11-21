#include <stdlib.h>

#include "include/geometry.h"
#include "include/panel.h"
#include "include/interface.h"


struct UserInterface {
    struct Panel *panel[NUM_UI_PANELS];
    bool show[NUM_UI_PANELS];
};


struct UserInterface *ui_create(void)
{
    struct UserInterface *ui = malloc(sizeof(struct UserInterface));
    if (!ui) {
        return NULL;
    }

    for (int i=0; i<NUM_UI_PANELS; i++) {
        ui->panel[i] = NULL;
        ui->show[i] = false;
    }

    return ui;
}


void ui_initialise(struct UserInterface *ui, struct Geometry *g)
{
    ui->panel[PANEL_SPLASH] = panel_splash();
    ui->panel[PANEL_TERRAIN] = panel_terrain_selector();
    ui->panel[PANEL_DETAIL] = panel_hex_detail();
    panel_centre(ui->panel[PANEL_SPLASH], geometry_rmid(g), geometry_cmid(g));
    ui->show[PANEL_SPLASH] = true;

    return;
}


void ui_destroy(struct UserInterface *ui)
{
    for (int i=0; i<NUM_UI_PANELS; i++) {
        struct Panel *panel = ui->panel[i];
        if (panel) {
            panel_destroy(panel);
        }
    }
    free(ui);
    ui = NULL;

    return;
}


void ui_toggle(struct UserInterface *ui, enum UI_PANEL p)
{
    ui->show[p] = !(ui->show[p]);

    return;
}


bool ui_show(struct UserInterface *ui, enum UI_PANEL p)
{
    return ui->show[p];
}


struct Panel *ui_panel(struct UserInterface *ui, enum UI_PANEL p)
{
    return ui->panel[p];
}
