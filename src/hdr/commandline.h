#ifndef COMMANDLINE_H
#define COMMANDLINE_H

#include "enum.h"

void commandline_reset(void);

size_t commandline_len(void);
enum COMMAND commandline_type(void);
const char *commandline_str(void);
char *commandline_curr(void);
const char *commandline_data(void);

void commandline_putch(char ch);
char commandline_popch(void);
void commandline_clearword(void);
void commandline_clearline(void);
void commandline_parse(void);

#endif
