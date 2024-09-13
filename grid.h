#ifndef GRID_H
#define GRID_H


enum TERRAIN {
    NONE,
    WATER,
    MOUNTAIN,
    PLAINS,
    HILL,
    FOREST,
    DESERT,
    JUNGLE,
    SWAMP
};


enum HexDirection {
    EAST,
    NORTHEAST,
    NORTHWEST,
    WEST,
    SOUTHWEST,
    SOUTHEAST
};


struct HexCoordinate {
    int x, y, z;
};


struct Hex {
    struct HexCoordinate p;
    enum TERRAIN t;
    struct Hex *n[6];
};


struct Hex *create_hex(void)
{
    struct Hex *h = (struct Hex *)malloc(sizeof(struct Hex));
    if (h == NULL) {
        return NULL;
    }

    h->p.x = 0;
    h->p.y = 0;
    h->p.z = 0;
    h->t = NONE;
    for (int i=0; i<6; i++) {
        h->n[i] = NULL;
    }

    return h;
}


void destroy_hex(struct Hex *h)
{
    free(h);
}


#endif
