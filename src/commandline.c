#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "hdr/commandline.h"

#define COMMANDLINE_BUFFER_SIZE 1024
#define COMMAND_WORD_QUIT   "quit"
#define COMMAND_WORD_WRITE  "write"
#define COMMAND_WORD_EDIT   "edit"


char buffer[COMMANDLINE_BUFFER_SIZE] = { 0 };
size_t len = 0;
size_t curr = 0;
enum COMMAND type = COMMAND_NONE;
char *data = NULL;


size_t commandline_len(void) { return len; }
const char *commandline_str(void) { return buffer; }
enum COMMAND commandline_type(void) { return type; }
const char *commandline_data(void) { return data; }


void commandline_reset(void)
{
    memset(buffer, 0, COMMANDLINE_BUFFER_SIZE);
    len = 0;
    curr = 0;
    type = COMMAND_NONE;
    if (data) free(data);
    data = NULL;
}


void commandline_putch(char ch)
{
    /* -1 offset so has null terminator */
    if (len >= COMMANDLINE_BUFFER_SIZE - 1) return;
    for (size_t i = len; i > curr; i--) {
        buffer[i] = buffer[i - 1];
    }
    buffer[curr++] = ch;
    len++;
}


char commandline_popch(void)
{
    if (len <= 0) return 0;
    if (curr <= 0) return 0;
    char tmp = buffer[curr - 1];
    for (size_t i = curr - 1; i < len - 1; i++) {
        buffer[i] = buffer[i + 1];
    }
    buffer[--len] = '\0';
    curr--;
    return tmp;
}


size_t commandline_curr(void) { return curr; }


char *commandline_next(void)
{
    if (curr >= len) return (buffer + len);
    return buffer + (++curr);
}


char *commandline_prev(void)
{
    if (curr <= 0) return (buffer);
    return buffer + (--curr);
}


char *commandline_to_next(int (test)(int))
{
    char *c = buffer + curr;
    while (!test(*c) && (curr < len)) c = commandline_next();
    return c;
}


bool commandline_match(char *keyword, char *c, size_t L)
{
    return ((L > 0) && (L <= strlen(keyword)) && !strncmp(keyword, c, L));
}


void commandline_clearword(void)
{
    if (len == 0) return;
    while (isspace(buffer[len - 1])) commandline_popch();
    while (isgraph(buffer[len - 1])) commandline_popch();
}


void commandline_clearline(void)
{
    memset(buffer, 0, COMMANDLINE_BUFFER_SIZE);
    len = 0;
    curr = 0;
}


void commandline_parse(void)
{
    curr = 0;

    char *c0 = commandline_to_next(isgraph);
    char *c1 = commandline_to_next(isspace);
    size_t L = c1 - c0;

    if (commandline_match(COMMAND_WORD_QUIT, c0, L))  {
        type = COMMAND_QUIT;
    } else if (commandline_match(COMMAND_WORD_WRITE, c0, L))  {
        type = COMMAND_WRITE;
    } else if (commandline_match(COMMAND_WORD_EDIT, c0, L))  {
        type = COMMAND_EDIT;
    } else {
        type = COMMAND_ERROR;
    }

    if ((type == COMMAND_QUIT) || (type == COMMAND_ERROR)) return;

    c0 = commandline_to_next(isgraph);
    c1 = commandline_to_next(isspace);
    L = c1 - c0;
    if (data) free(data);
    data = strndup(c0, L);
}
