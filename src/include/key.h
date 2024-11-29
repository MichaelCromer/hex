#ifndef KEY_H
#define KEY_H

#include <stdbool.h>

#include "enum.h"

#define              KEY_ESCAPE 27
#define        KEY_MODE_TERRAIN 'T'
#define  KEY_MODE_AWAIT_TERRAIN 't'
#define        KEY_MODE_COMMAND ':'

#define       KEY_TOGGLE_DETAIL 'j'
#define     KEY_TOGGLE_RETICULE 'x'


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
#define       KEY_TERRAIN_WATER 'q'
#define   KEY_TERRAIN_MOUNTAINS 'w'
#define       KEY_TERRAIN_HILLS 'e'
#define      KEY_TERRAIN_PLAINS 'a'
#define      KEY_TERRAIN_FOREST 's'
#define      KEY_TERRAIN_TUNDRA 'c'
#define      KEY_TERRAIN_DESERT 'z'
#define      KEY_TERRAIN_JUNGLE 'x'
#define       KEY_TERRAIN_SWAMP 'd'

typedef int key;

bool key_is_direction(key k);
bool key_is_special(key k);
bool key_is_terrain(key k);
bool key_is_mode(key k);

enum DIRECTION key_direction(key k);
enum TERRAIN key_terrain(key k);
enum INPUT_MODE key_mode(key k);

#endif
