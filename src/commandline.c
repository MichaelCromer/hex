#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "hdr/commandline.h"
#include "hdr/stringarray.h"

#define COMMANDLINE_BUFFER_SIZE 1024

char buffer[COMMANDLINE_BUFFER_SIZE] = { 0 };
size_t len = 0;
char *cursor = buffer;
enum COMMAND command = COMMAND_NONE;
char *data = NULL;


size_t commandline_len(void) { return len; }
const char *commandline_str(void) { return buffer; }
enum COMMAND commandline_command(void) { return command; }
const char *commandline_data(void) { return data; }
char *commandline_cursor(void) { return cursor; }


bool commandline_in_buffer(const char *ptr)
{
    return ((ptr >= buffer) && (ptr <= buffer + COMMANDLINE_BUFFER_SIZE));
}


void commandline_reset(void)
{
    memset(buffer, 0, COMMANDLINE_BUFFER_SIZE);
    len = 0;
    cursor = buffer;
    command = COMMAND_NONE;
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

    len = strlen(buffer);
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

    len = strlen(buffer);
    return len_paste;
}


size_t commandline_insert(char *fst, const char *str)
{
    if (!commandline_in_buffer(fst) || commandline_in_buffer(str)) return 0;

    size_t len_insert_max = COMMANDLINE_BUFFER_SIZE - (fst - buffer) - 1;
    size_t len_insert = (strlen(str) > len_insert_max) ? len_insert_max : strlen(str);

    strncpy(fst, str, len_insert);

    len = strlen(buffer);
    return len_insert;
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
        if (test(*curr)) return curr;
        if (curr == buffer + ((fwd) ? len : 0)) return NULL;
        curr = (fwd) ? curr + 1 : curr - 1;
    }
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


void commandline_cursor_cut_word(void)
{
    if (len == 0) return;
    char *fst = commandline_find_prev(cursor, isgraph);
    if (!fst) {
        commandline_cursor_cut_line();
        return;
    }
    while (commandline_in_buffer(fst - 1) && isgraph(*(fst - 1))) fst--;
    cursor -= commandline_cut(fst, cursor);
}


void commandline_cursor_cut_line(void)
{
    cursor -= commandline_cut(buffer, cursor);
}


int isnull(int ch) { return (0 == ch) ? 1 : 0; }


void commandline_parse(void)
{
    char *cmd_fst = NULL, *cmd_snd = NULL;
    ptrdiff_t len_cmd = -1;

    command = COMMAND_NONE;
    data = NULL;

    cmd_fst = commandline_find_next(buffer, isgraph);
    if (!cmd_fst) return;

    cmd_snd = commandline_find_next(cmd_fst, isspace);
    if (!cmd_snd) cmd_snd = commandline_find_next(cmd_fst, isnull);

    len_cmd = cmd_snd - cmd_fst;
    if (!len_cmd) return;

    if (commandline_match(cmd_fst, command_str(COMMAND_QUIT), len_cmd))  {
        command = COMMAND_QUIT;
    } else if (commandline_match(cmd_fst, command_str(COMMAND_WRITE), len_cmd))  {
        command = COMMAND_WRITE;
    } else if (commandline_match(cmd_fst, command_str(COMMAND_EDIT), len_cmd))  {
        command = COMMAND_EDIT;
    } else {
        command = COMMAND_ERROR;
    }

    if ((command == COMMAND_QUIT) || (command == COMMAND_ERROR)) return;

    data = commandline_find_next(cmd_snd, isgraph);
}


void commandline_complete_keyword(void)
{
    const char *str = command_str(command);
    char *fst = buffer;

    while (!isgraph(*fst)) fst++;
    cursor = fst + commandline_insert(fst, str);
}


void commandline_complete_path(const char *str_basename)
{
    if (!str_basename || !data) return;
    if (!commandline_in_buffer(data)) return;

    char *str_fullpath = NULL, *str_filename = NULL, *common = NULL;
    struct StringArray *dirent = NULL;

    str_fullpath = malloc(strlen(str_basename) + strlen(data) + 2);
    strcpy(str_fullpath, str_basename);
    strcat(str_fullpath, "/");
    strcat(str_fullpath, data);
    char *tmp = strrchr(str_fullpath, '/');
    if (tmp) *tmp = '\0';

    str_filename = strrchr(data, '/');
    if (!str_filename) str_filename = data;
    if ('/' == *str_filename) str_filename += 1;

    dirent = strarr_dir_filenames(str_fullpath);
    common = strarr_common_prefix(dirent, str_filename);

    cursor = str_filename + commandline_insert(str_filename, common);

    strarr_destroy(dirent);
    free(common);
    free(str_fullpath);
}


void commandline_complete(const char *str_basename)
{
    commandline_parse();

    if ((!data) && (COMMAND_NONE != command) && (COMMAND_ERROR != command)) {
        commandline_complete_keyword();
        return;
    }

    if (data && ((COMMAND_WRITE == command) || (COMMAND_EDIT == command))) {
        commandline_complete_path(str_basename);
    }
}
