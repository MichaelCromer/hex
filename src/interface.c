#include <stdlib.h>
#include <string.h>

#include "include/geometry.h"
#include "include/atlas.h"
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
    ui->panel[PANEL_DETAIL] = panel_tile_detail();
    panel_centre(ui->panel[PANEL_SPLASH], geometry_rmid(g), geometry_cmid(g));
    ui->show[PANEL_SPLASH] = true;
}


void ui_update_detail(struct UserInterface *ui, struct Chart *chart)
{
    struct Panel *detail = ui_panel(ui, PANEL_DETAIL);
    char buf[32];

    panel_remove_line(detail, 1);
    memset(buf, 0 ,32);
    snprintf(buf, 32, "  (%d, %d, %d)", chart_p(chart), chart_q(chart), chart_r(chart));
    panel_add_line(detail, 1, buf);

    panel_remove_line(detail, 2);
    memset(buf, 0 ,32);
    snprintf(buf, 32, "  Terrain: %s", terrain_name(chart_terrain(chart)));
    panel_add_line(detail, 2, buf);
}


void ui_destroy(struct UserInterface *ui)
{
    for (int i = 0; i < NUM_UI_PANELS; i++) {
        struct Panel *panel = ui->panel[i];
        if (panel) {
            panel_destroy(panel);
        }
    }
    free(ui);
}


void ui_toggle(struct UserInterface *ui, enum UI_PANEL p)
{
    ui->show[p] = !(ui->show[p]);
}


bool ui_show(struct UserInterface *ui, enum UI_PANEL p)
{
    return ui->show[p];
}


struct Panel *ui_panel(struct UserInterface *ui, enum UI_PANEL p)
{
    return ui->panel[p];
}
