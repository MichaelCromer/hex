#ifndef CELL_H
#define CELL_H


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


struct HexCoordinate {
    int x;
    int y;
    int z;
};


struct Hex {
    struct HexCoordinate p;
    enum TERRAIN t;
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

    return h;
}


void destroy_hex(struct Hex *h)
{
    free(h);
}



#endif
