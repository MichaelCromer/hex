#ifndef STRINGARRAY_H
#define STRINGARRAY_H

#include <stdio.h>

struct StringArray;
struct StringArray *strarr_create(void);
void strarr_destroy(struct StringArray *strarr);
void strarr_fprint(FILE *file, struct StringArray *strarr);
void strarr_add(struct StringArray *strarr, const char *str_new);
struct StringArray *strarr_dir_filenames(const char *str_path);
char *strarr_common_prefix(struct StringArray *strarr, const char *str_match);

#endif
