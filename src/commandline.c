#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "hdr/commandline.h"

#define COMMANDLINE_BUFFER_SIZE 1024

const char *COMMAND_WORD_QUIT = "quit";
const char *COMMAND_WORD_WRITE = "write";
const char *COMMAND_WORD_EDIT = "edit";
const char *COMMAND_WORD_NONE = "";


char buffer[COMMANDLINE_BUFFER_SIZE] = { 0 };
size_t len = 0;
char *cursor = buffer;
enum COMMAND command = COMMAND_NONE;
char *data = NULL;


const char *command_str(enum COMMAND c)
{
    switch (c) {
        case COMMAND_QUIT:
            return COMMAND_WORD_QUIT;
        case COMMAND_WRITE:
            return COMMAND_WORD_WRITE;
        case COMMAND_EDIT:
            return COMMAND_WORD_EDIT;
        default:
            break;
    }

    return COMMAND_WORD_NONE;
}


bool commandline_in_buffer(const char *ptr)
{
    return ((ptr >= buffer) && (ptr <= buffer + COMMANDLINE_BUFFER_SIZE));
}


size_t commandline_len(void) { return len; }
const char *commandline_str(void) { return buffer; }
enum COMMAND commandline_command(void) { return command; }
const char *commandline_data(void) { return data; }
char *commandline_cursor(void) { return cursor; }


void commandline_reset(void)
{
    memset(buffer, 0, COMMANDLINE_BUFFER_SIZE);
    len = 0;
    cursor = buffer;
    command = COMMAND_NONE;
    if (data) free(data);
    data = NULL;
}


size_t commandline_cursor_pos(void)
{
    ptrdiff_t dp = cursor - buffer;
    size_t pos = (0 <= dp) ? (size_t)dp : 0;
    return (pos <= len) ? pos : len;
}


size_t commandline_cut(char *fst, char *snd)
{
    if (!commandline_in_buffer(fst) || !commandline_in_buffer(snd) || (snd < fst)) {
        return 0;
    }

    if (fst > buffer + len) return 0;
    if (snd > buffer + len) snd = buffer + len;

    size_t len_cut = (snd - fst);
    size_t len_tail = COMMANDLINE_BUFFER_SIZE - (snd - buffer);

    memmove(fst, snd, len_tail);
    memset(fst + len_tail, 0, len_cut);

    len -= len_cut;
    return len_cut;
}


size_t commandline_paste(char *fst, const char *str)
{
    if (!commandline_in_buffer(fst) || commandline_in_buffer(str)) return 0;

    size_t len_paste_max = COMMANDLINE_BUFFER_SIZE - (fst - buffer) - 1;
    size_t len_paste = (strlen(str) > len_paste_max) ? len_paste_max : strlen(str);
    size_t len_tail = len_paste_max - len_paste;

    memmove(fst + len_paste, fst, len_tail);
    memcpy(fst, str, len_paste);

    len += len_paste;
    return len_paste;
}


void commandline_cursor_putch(char ch)
{
    if (commandline_cursor_pos() == COMMANDLINE_BUFFER_SIZE - 1) return;
    char tmp[2] = { ch, '\0' };
    cursor += commandline_paste(cursor, tmp);
}


char commandline_cursor_popch(void)
{
    if (commandline_cursor_pos() == 0) return 0;
    char tmp = *(cursor - 1);
    cursor -= commandline_cut(cursor - 1, cursor);
    return tmp;
}


char *commandline_find(char *fst, int (*test)(int), bool fwd)
{
    if (!commandline_in_buffer(fst)) return NULL;
    char *curr = fst;

    while (1) {
        if (test(*curr) || (curr == buffer + ((fwd) ? len : 0))) break;
        curr = (fwd) ? curr + 1 : curr - 1;
    }

    return curr;
}


char *commandline_find_prev(char *fst, int (*test)(int))
{
    return commandline_find(fst, test, false);
}


char *commandline_find_next(char *fst, int (*test)(int))
{
    return commandline_find(fst, test, true);
}


char *commandline_cursor_next(void)
{
    if (commandline_cursor_pos() == len) return cursor;
    return ++cursor;
}


char *commandline_cursor_prev(void)
{
    if (commandline_cursor_pos() == 0) return cursor;
    return --cursor;
}


bool commandline_match(char *fst, const char *keyword, size_t n)
{
    if (!commandline_in_buffer(fst) || !commandline_in_buffer(fst + n)) return false;
    return ((n <= strlen(keyword)) && (0 == strncmp(keyword, fst, n)));
}


void commandline_cursor_clearword(void)
{
    if (len == 0) return;
    char *fst = commandline_find_prev(cursor, isspace);
    fst = commandline_find_prev(fst, isgraph);
    while (commandline_in_buffer(fst - 1) && isgraph(*(fst - 1))) fst--;
    cursor -= commandline_cut(fst, cursor);
}


void commandline_cursor_clearline(void)
{
    cursor -= commandline_cut(buffer, cursor);
}


void commandline_parse(void)
{
    char *cmd_fst = NULL, *cmd_snd = NULL;
    char *data_fst = NULL;
    ptrdiff_t len_cmd = 0, len_data = 0;

    command = COMMAND_NONE;
    if (data) free(data);
    data = NULL;

    cmd_fst = commandline_find_next(buffer, isgraph);
    cmd_snd = commandline_find_next(cmd_fst, isspace);
    len_cmd = cmd_snd - cmd_fst;

    if (commandline_match(cmd_fst, COMMAND_WORD_QUIT, len_cmd))  {
        command = COMMAND_QUIT;
    } else if (commandline_match(cmd_fst, COMMAND_WORD_WRITE, len_cmd))  {
        command = COMMAND_WRITE;
    } else if (commandline_match(cmd_fst, COMMAND_WORD_EDIT, len_cmd))  {
        command = COMMAND_EDIT;
    } else {
        command = COMMAND_ERROR;
    }

    if ((command == COMMAND_QUIT) || (command == COMMAND_ERROR)) return;

    data_fst = commandline_find_next(cmd_snd, isgraph);
    len_data = (buffer + COMMANDLINE_BUFFER_SIZE) - data_fst;
    if (len_data) data = strndup(data_fst, len_data);
}


void commandline_complete_keyword(void)
{
    const char *str = command_str(command);
    char *fst = buffer;

    while (!isgraph(*fst)) fst++;
    while ((*fst == *str) && (*str)) fst++, str++;
    commandline_paste(fst, str);
}


void commandline_complete(void)
{
    commandline_parse();

    if ((COMMAND_NONE != command) && (COMMAND_ERROR != command) && (!data)) {
        commandline_complete_keyword();
        return;
    }
}
