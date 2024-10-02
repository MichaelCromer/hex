#include "grid.h"


struct Hex {
    int x, y, z;
    enum TERRAIN t;
    struct Hex *n[6];
};


struct Hex *hex_create(void)
{
    struct Hex *h = (struct Hex *)malloc(sizeof(struct Hex));
    if (h == NULL) {
        return NULL;
    }

    h->x = 0;
    h->y = 0;
    h->z = 0;
    h->t = NONE;
    for (int i=0; i<6; i++) {
        h->n[i] = NULL;
    }

    return h;
}


void hex_destroy(struct Hex *h)
{
    free(h);
}


void hex_set_terrain(struct Hex *h, enum TERRAIN t)
{
    h->t = t;
}


enum TERRAIN hex_get_terrain(struct Hex *h)
{
    return h->t;
}


void hex_set_coords(struct Hex *h, int x, int y, int z)
{
    h->x = x;
    h->y = y;
    h->z = z;
}

int hex_x(struct Hex *h)
{
    return h->x;
}

int hex_y(struct Hex *h)
{
    return h->y;
}

int hex_z(struct Hex *h)
{
    return h->z;
}
