#ifndef COMMANDLINE_H
#define COMMANDLINE_H

#include "enum.h"

void commandline_reset(void);

size_t commandline_len(void);
enum COMMAND commandline_command(void);
const char *commandline_str(void);
const char *commandline_data(void);

void commandline_cursor_putch(char ch);
char commandline_cursor_popch(void);
void commandline_cursor_cut_word(void);
void commandline_cursor_cut_line(void);
size_t commandline_len(void);
char *commandline_cursor(void);
size_t commandline_cursor_pos(void);
char *commandline_cursor_next(void);
char *commandline_cursor_prev(void);
void commandline_parse(void);
void commandline_complete(void);

#endif
