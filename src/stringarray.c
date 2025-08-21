#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>


#define STRARR_GROWTH_FACTOR (3/2)


struct StringArray
{
    size_t len_curr;
    size_t len_max;
    char **strings;
};


struct StringArray *strarr_create(void)
{
    struct StringArray *strarr = malloc(sizeof(struct StringArray));
    strarr->len_curr = 0;
    strarr->len_max = 0;
    strarr->strings = NULL;

    return strarr;
}


void strarr_destroy(struct StringArray *strarr)
{
    if (!strarr) return;
    for (size_t i = 0; i < strarr->len_curr; i++) {
        free(strarr->strings[i]);
        strarr->strings[i] = NULL;
    }

    free(strarr->strings);
    strarr->strings = NULL;
    strarr->len_curr = 0;
    strarr->len_max = 0;
    free(strarr);
}


void strarr_fprint(FILE *file, struct StringArray *strarr)
{
    for (size_t i = 0; i < strarr->len_curr; i++) {
        fprintf(file, "%s\n", strarr->strings[i]);
    }
}


void strarr_add(struct StringArray *strarr, const char *str_new)
{
    if (strarr->len_curr >= strarr->len_max) {
        size_t len_new = 1 + (STRARR_GROWTH_FACTOR * strarr->len_max);
        char **tmp = realloc(strarr->strings, len_new * sizeof(char *));
        if (!tmp) return;
        strarr->strings = tmp;
        strarr->len_max = len_new;
    }

    strarr->strings[strarr->len_curr++] = strdup(str_new);
}


struct StringArray *strarr_dir_filenames(const char *str_path)
{
    DIR *dir = NULL;
    struct dirent *dir_entry = NULL;
    struct StringArray *strarr = strarr_create();

    dir = opendir(str_path);
    if (NULL == dir) return NULL;

    while (NULL != (dir_entry = readdir(dir))) {
        strarr_add(strarr, dir_entry->d_name);
    }

    closedir(dir);
    return strarr;
}


char *strarr_common_prefix(struct StringArray *strarr, const char *str_match)
{
    char *common = NULL;
    size_t len_match = strlen(str_match);
    size_t len_accept = 0;

    for (size_t i = 0; i < strarr->len_curr; i++) {
        if (0 != strncmp(strarr->strings[i], str_match, len_match)) continue;

        if (!common) {
            common = strdup(strarr->strings[i]);
        } else {
            len_accept = strspn(common, strarr->strings[i]);
            common[len_accept] = '\0';
        }
    }

    if (!common) common = strdup("");

    return common;
}
