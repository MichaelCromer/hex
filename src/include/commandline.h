#ifndef COMMANDLINE_H
#define COMMANDLINE_H

#include "enum.h"

struct Command;
struct Commandline;

enum COMMAND command_type(const struct Command *c);
char *command_data(const struct Command *c);
void command_destroy(struct Command *c);

void commandline_reset(struct Commandline *c);
struct Commandline *commandline_create(void);
void commandline_destroy(struct Commandline *c);
size_t commandline_len(struct Commandline *c);
const char *commandline_str(struct Commandline *c);
void commandline_putch(struct Commandline *c, char ch);
char commandline_popch(struct Commandline *c);
struct Command *commandline_parse(struct Commandline *c);

#endif
