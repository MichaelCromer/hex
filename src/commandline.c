#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "include/commandline.h"

#define COMMANDLINE_BUFFER_SIZE 1024
#define COMMAND_WORD_QUIT   "quit"


struct Command {
    enum COMMAND type;
    char *data;
};


struct Command *command_create(enum COMMAND type, char *data)
{
    struct Command *c = malloc(sizeof(struct Command));
    c->type = type;
    c->data = data;

    return c;
}


enum COMMAND command_type(const struct Command *c)
{
    return c->type;
}


void command_destroy(struct Command *c)
{
    if (c->data) {
        free(c->data);
        c->data = NULL;
    }
    free(c);
}


struct Commandline {
    char buffer[COMMANDLINE_BUFFER_SIZE];
    int len;
    char *curr;
};


struct Commandline *commandline_create(void)
{
    struct Commandline *c = malloc(sizeof(struct Commandline));
    commandline_reset(c);
    return c;
}


void commandline_reset(struct Commandline *c)
{
    memset(c->buffer, '\0', COMMANDLINE_BUFFER_SIZE);
    c->len = 0;
    c->curr = c->buffer;
}


void commandline_destroy(struct Commandline *c)
{
    free(c);
}


size_t commandline_len(struct Commandline *c)
{
    return c->len;
}


const char *commandline_str(struct Commandline *c)
{
    return c->buffer;
}


void commandline_putch(struct Commandline *c, char ch)
{
    if (c->len >= COMMANDLINE_BUFFER_SIZE - 1) {
        return;
    }

    c->buffer[(c->len)] = ch;
    (c->len)++;
}


char commandline_popch(struct Commandline *c)
{
    if (c->len <= 0) {
        return '\0';
    }
    char ch = c->buffer[(c->len)-1];
    (c->len)--;
    c->buffer[(c->len)] = '\0';
    return ch;
}


char *commandline_start(struct Commandline *c)
{
    return (c->curr = c->buffer);
}


char *commandline_next(struct Commandline *c)
{
    if (c->curr >= c->buffer + c->len + 1) {
        return c->buffer + c->len + 1;
    }
    return (c->curr++);
}


char *commandline_curr(struct Commandline *c)
{
    return c->curr;
}


enum COMMAND commandline_parse_type(struct Commandline *c)
{
    char *c0 = commandline_curr(c);
    while (isspace(*c0) && (*c0 != '\0')) {
        c0 = commandline_next(c);
    }

    char *c1 = commandline_curr(c);
    while (!isspace(*c1) && (*c1 != '\0')) {
        c1 = commandline_next(c);
    }

    size_t L = c1 - c0;

    if (L == 0) {
        return COMMAND_NONE;
    }

    if ((L <= strlen(COMMAND_WORD_QUIT)) && (strncmp(COMMAND_WORD_QUIT, c0, L) == 0)) {
        return COMMAND_QUIT;
    }

    return COMMAND_ERROR;
}


char *commandline_parse_data(struct Commandline *c)
{
    char *c0 = commandline_curr(c);
    while (isspace(*c0) && (*c0 != '\0')) {
        c0 = commandline_next(c);
    }

    char *c1 = commandline_curr(c);
    while (!isspace(*c1) && (*c1 != '\0')) {
        c1 = commandline_next(c);
    }

    int L = c1 - c0;
    char *data = malloc(L + 1);

    for (int i = 0; i < L; i++) {
        data[i] = *(c0 + i);
    }
    data[L] = '\0';

    return data;
}


struct Command *commandline_parse(struct Commandline *c)
{
    commandline_start(c);

    enum COMMAND t = commandline_parse_type(c);
    
    switch (t) {
        case COMMAND_QUIT:
        case COMMAND_ERROR:
            return command_create(t, NULL);
        case COMMAND_EDIT:
        case COMMAND_WRITE:
        default:
            break;
    }

    return command_create(COMMAND_NONE, NULL);
}
