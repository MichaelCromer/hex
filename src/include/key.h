#ifndef KEY_H
#define KEY_H

#include <stdbool.h>

#include "enum.h"

#define         KEY_TOGGLE_QUIT 'Q'
#define      KEY_TOGGLE_TERRAIN 'T'
#define       KEY_TOGGLE_DETAIL 'j'

#define                  KEY_EE 'k'
#define                  KEY_NE 'i'
#define                  KEY_NW 'u'
#define                  KEY_WW 'h'
#define                  KEY_SW 'n'
#define                  KEY_SE 'm'

#define          KEY_EE_SPECIAL 'K'
#define          KEY_NE_SPECIAL 'I'
#define          KEY_NW_SPECIAL 'U'
#define          KEY_WW_SPECIAL 'H'
#define          KEY_SW_SPECIAL 'N'
#define          KEY_SE_SPECIAL 'M'

#define     KEY_TERRAIN_UNKNOWN '0'
#define       KEY_TERRAIN_WATER '1'
#define   KEY_TERRAIN_MOUNTAINS '2'
#define      KEY_TERRAIN_PLAINS '3'
#define       KEY_TERRAIN_HILLS '4'
#define      KEY_TERRAIN_FOREST '5'
#define      KEY_TERRAIN_DESERT '6'
#define      KEY_TERRAIN_JUNGLE '7'
#define       KEY_TERRAIN_SWAMP '8'

enum DIRECTION key_direction(char ch);
bool key_is_direction(char ch);
bool key_is_special(char ch);
enum TERRAIN key_terrain(char ch);
enum TERRAIN key_is_terrain(char ch);

#endif
