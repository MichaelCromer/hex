#include <stdlib.h>
#include <string.h>

#include "include/atlas.h"
#include "include/interface.h"
#include "include/panel.h"
#include "include/state.h"


struct Panel *panel[NUM_UI_PANELS] = { 0 };
bool show[NUM_UI_PANELS] = { 0 };


void ui_initialise(void)
{
    panel[PANEL_SPLASH] = panel_splash();
    panel[PANEL_DETAIL] = panel_tile_detail();
    panel[PANEL_HINT] = panel_hint();
    panel[PANEL_NAVIGATE] = panel_navigate();
    panel[PANEL_TERRAIN] = panel_terrain();

    panel_centre(panel[PANEL_SPLASH], geometry_rmid(), geometry_cmid());

    panel_set_rc(
        panel[PANEL_HINT],
        1,
        geometry_cols() - panel_width(panel[PANEL_HINT]) - 1
    );

    panel_set_rc(
        panel[PANEL_NAVIGATE],
        panel_height(panel[PANEL_HINT]),
        geometry_cols() - panel_width(panel[PANEL_NAVIGATE]) - 1
    );

    panel_set_rc(
        panel[PANEL_TERRAIN],
        panel_height(panel[PANEL_HINT]),
        geometry_cols() - panel_width(panel[PANEL_TERRAIN]) - 1
    );

    show[PANEL_SPLASH] = true;
}


void ui_update(struct State *state)
{
    /*
     *  tile detail panel
     */
    struct Atlas *atlas = state_atlas(state);
    struct Chart *chart = atlas_curr(atlas);
    struct Panel *detail = panel[PANEL_DETAIL];

    char buf[32];

    memset(buf, 0, 32);
    panel_remove_line(detail, 1);
    snprintf(buf, 32, "  (%d, %d, %d)", chart_p(chart), chart_q(chart), chart_r(chart));
    panel_add_line(detail, 1, buf);

    memset(buf, 0, 32);
    panel_remove_line(detail, 2);
    snprintf(buf, 32, "  Terrain: %s", terrain_name(atlas_terrain(atlas)));
    panel_add_line(detail, 2, buf);

    /*
     * help/hint panels
     */
    if (!show[PANEL_HINT]) return;
    show[PANEL_NAVIGATE] = false;
    show[PANEL_TERRAIN] = false;
    show[mode_panel(state_mode(state))] = true;
}


void ui_toggle_show(enum UI_PANEL p)
{
    if (p == PANEL_NONE) return;
    show[p] = !(show[p]);
}


bool ui_is_show(enum UI_PANEL p)
{
    if (p == PANEL_NONE) return false;
    return show[p];
}


struct Panel *ui_panel(enum UI_PANEL p)
{
    if (p == PANEL_NONE) return NULL;
    return panel[p];
}
