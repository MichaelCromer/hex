#ifndef COMMANDLINE_H
#define COMMANDLINE_H

#include "enum.h"

struct Command;
struct Commandline;

struct Command *command_create(enum COMMAND type, char *data);
enum COMMAND command_type(const struct Command *c);
const char *command_str(const struct Command *c);
void command_destroy(struct Command *c);

void commandline_reset(struct Commandline *c);
struct Commandline *commandline_create(void);
void commandline_destroy(struct Commandline *c);
size_t commandline_len(struct Commandline *c);
const char *commandline_str(struct Commandline *c);
void commandline_putch(struct Commandline *c, char ch);
char commandline_popch(struct Commandline *c);
char *commandline_start(struct Commandline *c);
char *commandline_end(struct Commandline *c);
char *commandline_next(struct Commandline *c);
char *commandline_prev(struct Commandline *c);
char *commandline_curr(struct Commandline *c);
enum COMMAND commandline_parse_type(struct Commandline *c);
char *commandline_parse_data(struct Commandline *c);
struct Command *commandline_parse(struct Commandline *c);


#endif
