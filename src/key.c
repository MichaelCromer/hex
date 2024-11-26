#include <ctype.h>
#include <ncurses.h>

#include "include/key.h"


enum KEY key_parse(enum INPUT_MODE m, int c)
{
    /* handle escape codes here */
    if (c == 27) {
        return KEY_ESCAPE;
    }

    /* handle universal inputs here */
    switch (c) {
        case 'j':
            return KEY_TOGGLE_DETAIL;
        case 'k':
            return KEY_STEP_EE;
        case 'i':
            return KEY_STEP_NE;
        case 'u':
            return KEY_STEP_NW;
        case 'h':
            return KEY_STEP_WW;
        case 'n':
            return KEY_STEP_SW;
        case 'm':
            return KEY_STEP_SE;
        case ':':
            return KEY_MODE_COMMAND;
        default:
            break;
    }

    switch (m) {
        case INPUT_MODE_NAVIGATE:
            return key_parse_navigate(c);
        case INPUT_MODE_TERRAIN:
            return key_parse_terrain(c);
        case INPUT_MODE_CAPTURE:
            return key_parse_capture(c);
        default:
            break;

    }

    return KEY_NONE;
}


enum KEY key_parse_escseq(int c)
{
    switch (c) {
        case -1:
            return KEY_MODE_NAVIGATE;
        default:
            break;
    }

    return KEY_NONE;
}


enum KEY key_parse_capture(int c)
{
    switch (c) {
        case KEY_ENTER:
        case '\n':
            return KEY_MODE_NAVIGATE;
        default:
            break;
    }
    return KEY_NONE;;
}


enum KEY key_parse_navigate(int c)
{
    switch (c) {
        case 't':
            return KEY_MODE_AWAIT_TERRAIN;
        case 'T':
            return KEY_MODE_TERRAIN;
        case 'K':
            return KEY_JUMP_EE;
        case 'I':
            return KEY_JUMP_NE;
        case 'U':
            return KEY_JUMP_NW;
        case 'H':
            return KEY_JUMP_WW;
        case 'N':
            return KEY_JUMP_SW;
        case 'M':
            return KEY_JUMP_SE;
        default:
            break;
    }
    return KEY_NONE;
}


enum KEY key_parse_terrain(int c)
{
    switch (c) {
        case 'T':
            return KEY_MODE_NAVIGATE;
        case '0':
            return KEY_PAINT_UNKNOWN;
        case 'q':
            return KEY_PAINT_WATER;
        case 'w':
            return KEY_PAINT_MOUNTAINS;
        case 'e':
            return KEY_PAINT_HILLS;
        case 'a':
            return KEY_PAINT_PLAINS;
        case 's':
            return KEY_PAINT_FOREST;
        case 'd':
            return KEY_PAINT_SWAMP;
        case 'z':
            return KEY_PAINT_DESERT;
        case 'x':
            return KEY_PAINT_JUNGLE;
        case 'c':
            return KEY_PAINT_TUNDRA;
        case 'K':
            return KEY_DRAG_EE;
        case 'I':
            return KEY_DRAG_NE;
        case 'U':
            return KEY_DRAG_NW;
        case 'H':
            return KEY_DRAG_WW;
        case 'N':
            return KEY_DRAG_SW;
        case 'M':
            return KEY_DRAG_SE;
        default:
            break;
    }
    return KEY_NONE;
}


enum DIRECTION key_direction(enum KEY k)
{
    switch (k) {
        case KEY_STEP_EE:
        case KEY_JUMP_EE:
        case KEY_DRAG_EE:
            return EAST;
        case KEY_STEP_NE:
        case KEY_JUMP_NE:
        case KEY_DRAG_NE:
            return NORTHEAST;
        case KEY_STEP_NW:
        case KEY_JUMP_NW:
        case KEY_DRAG_NW:
            return NORTHWEST;
        case KEY_STEP_WW:
        case KEY_JUMP_WW:
        case KEY_DRAG_WW:
            return WEST;
        case KEY_STEP_SW:
        case KEY_JUMP_SW:
        case KEY_DRAG_SW:
            return SOUTHWEST;
        case KEY_STEP_SE:
        case KEY_JUMP_SE:
        case KEY_DRAG_SE:
            return SOUTHEAST;
        default:
            break;
    }
    return DIRECTION_NONE;
}


bool key_is_step(enum KEY k)
{
    switch (k) {
        case KEY_STEP_EE:
        case KEY_STEP_NE:
        case KEY_STEP_NW:
        case KEY_STEP_WW:
        case KEY_STEP_SW:
        case KEY_STEP_SE:
            return true;
        default:
            break;
    }
    return false;
}


bool key_is_jump(enum KEY k)
{
    switch (k) {
        case KEY_JUMP_EE:
        case KEY_JUMP_NE:
        case KEY_JUMP_NW:
        case KEY_JUMP_WW:
        case KEY_JUMP_SW:
        case KEY_JUMP_SE:
            return true;
        default:
            break;
    }
    return false;
}


bool key_is_drag(enum KEY k)
{
    switch (k) {
        case KEY_DRAG_EE:
        case KEY_DRAG_NE:
        case KEY_DRAG_NW:
        case KEY_DRAG_WW:
        case KEY_DRAG_SW:
        case KEY_DRAG_SE:
            return true;
        default:
            break;
    }
    return false;
}


bool key_is_move(enum KEY k)
{
    if (key_is_step(k) || key_is_jump(k) || key_is_drag(k)) {
        return true;
    }
    return false;
}


int key_move_amount(enum KEY k)
{
    if (key_is_step(k) || key_is_drag(k)) {
        return 1;
    } else if (key_is_jump(k)) {
        return 3;
    } else {
        return 0;
    }
}


enum TERRAIN key_terrain(enum KEY k)
{
    switch (k) {
        case KEY_PAINT_UNKNOWN:
            return TERRAIN_UNKNOWN;
        case KEY_PAINT_WATER:
            return TERRAIN_WATER;
        case KEY_PAINT_MOUNTAINS:
            return TERRAIN_MOUNTAINS;
        case KEY_PAINT_PLAINS:
            return TERRAIN_PLAINS;
        case KEY_PAINT_HILLS:
            return TERRAIN_HILLS;
        case KEY_PAINT_FOREST:
            return TERRAIN_FOREST;
        case KEY_PAINT_DESERT:
            return TERRAIN_DESERT;
        case KEY_PAINT_JUNGLE:
            return TERRAIN_JUNGLE;
        case KEY_PAINT_SWAMP:
            return TERRAIN_SWAMP;
        case KEY_PAINT_TUNDRA:
            return TERRAIN_TUNDRA;
        default:
            break;
    }
    return TERRAIN_NONE;
}


bool key_is_paint(enum KEY k)
{
    switch (k) {
        case KEY_PAINT_UNKNOWN:
        case KEY_PAINT_WATER:
        case KEY_PAINT_MOUNTAINS:
        case KEY_PAINT_PLAINS:
        case KEY_PAINT_HILLS:
        case KEY_PAINT_FOREST:
        case KEY_PAINT_DESERT:
        case KEY_PAINT_JUNGLE:
        case KEY_PAINT_SWAMP:
        case KEY_PAINT_TUNDRA:
            return true;
        default:
            break;
    }
    return false;
}


bool key_is_mode(enum KEY k)
{
    switch (k) {
        case KEY_MODE_COMMAND:
        case KEY_MODE_NAVIGATE:
        case KEY_MODE_AWAIT_TERRAIN:
        case KEY_MODE_TERRAIN:
            return true;
        default:
            break;
    }
    return false;
}


enum INPUT_MODE key_mode(enum KEY k)
{
    switch (k) {
        case KEY_MODE_NAVIGATE:
            return INPUT_MODE_NAVIGATE;
        case KEY_MODE_COMMAND:
            return INPUT_MODE_COMMAND;
        case KEY_MODE_AWAIT_TERRAIN:
        case KEY_MODE_TERRAIN:
            return INPUT_MODE_TERRAIN;
        default:
            break;
    }
    return INPUT_MODE_NONE;
}


bool key_is_await(enum KEY k)
{
    switch (k) {
        case KEY_MODE_AWAIT_TERRAIN:
            return true;
        default:
            break;
    }
    return false;
}

