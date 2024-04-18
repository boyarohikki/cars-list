#ifndef COURSE_TASK_MENU_FUNCTIONS_SEARCH_FUNCS
#define COURSE_TASK_MENU_FUNCTIONS_SEARCH_FUNCS
#include <stddef.h>
#include <string.h>

#include "../structs.h"

/* Non-deep search in list by field of dynamic size with additional string
search feature */
char search(const ListHead list, void *field_value, size_t field_size,
            void *(*field_getter)(void *), void **(*next_getter)(void *),
            void (*printer)(const void *), char is_str) {
    void *iter = list.first;
    char found = 0;
    while (iter) {
        if (is_str) {
            if (strstr(*((char **)field_getter(iter)), (char *)field_value) !=
                NULL) {
                printer(iter);
                found = 1;
            }
        } else {
            if (memcmp(field_value, field_getter(iter), field_size) == 0) {
                printer(iter);
                found = 1;
            }
        }
        iter = *next_getter(iter);
    }
    return found;
}

#endif