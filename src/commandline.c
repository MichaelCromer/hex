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
enum COMMAND command = COMMAND_NONE;
char *data = NULL;


size_t commandline_len(void) { return len; }
const char *commandline_str(void) { return buffer; }
enum COMMAND commandline_command(void) { return command; }
const char *commandline_data(void) { return data; }


void commandline_reset(void)
{
    memset(buffer, 0, COMMANDLINE_BUFFER_SIZE);
    len = 0;
    curr = 0;
    command = COMMAND_NONE;
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


char *commandline_find(int (test)(int))
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
    char *c0 = NULL, *c1 = NULL;
    size_t L = 0;

    curr = 0;
    if (data) free(data);

    c0 = commandline_find(isgraph);
    c1 = commandline_find(isspace);
    L = c1 - c0;

    if (commandline_match(COMMAND_WORD_QUIT, c0, L))  {
        command = COMMAND_QUIT;
    } else if (commandline_match(COMMAND_WORD_WRITE, c0, L))  {
        command = COMMAND_WRITE;
    } else if (commandline_match(COMMAND_WORD_EDIT, c0, L))  {
        command = COMMAND_EDIT;
    } else {
        command = COMMAND_ERROR;
    }

    if ((command == COMMAND_QUIT) || (command == COMMAND_ERROR)) return;

    c0 = commandline_find(isgraph);
    c1 = commandline_find(isspace);
    L = c1 - c0;

    if (L) { data = strndup(c0, L); }
}
