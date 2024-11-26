#ifndef KEY_H
#define KEY_H

#include <stdbool.h>

#include "enum.h"

enum KEY key_parse(enum INPUT_MODE m, int c);
enum KEY key_parse_escseq(int c);
enum KEY key_parse_capture(int c);
enum KEY key_parse_navigate(int c);
enum KEY key_parse_terrain(int c);
enum DIRECTION key_direction(enum KEY k);
bool key_is_step(enum KEY k);
bool key_is_jump(enum KEY k);
bool key_is_drag(enum KEY k);
bool key_is_move(enum KEY k);
int key_move_amount(enum KEY k);
enum TERRAIN key_terrain(enum KEY k);
bool key_is_paint(enum KEY k);
bool key_is_mode(enum KEY k);
enum INPUT_MODE key_mode(enum KEY k);
bool key_is_await(enum KEY k);

#endif
