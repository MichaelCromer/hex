#ifndef CELL_H
#define CELL_H


enum TERRAIN {
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
    struct HexCoordinate coordinate;
    enum TERRAIN terrain;
};


#endif
