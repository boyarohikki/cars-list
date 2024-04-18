#ifndef COURSE_TASK_MENU_FUNCTIONS_UTILS
#define COURSE_TASK_MENU_FUNCTIONS_UTILS
#include <stddef.h>

#include "../getters.h"
#include "../structs.h"

/* Finds element which *id_getter(element) equals to id parameter */
void *find_element_with_id(ListHead list, unsigned id,
                           unsigned *(*id_getter)(void *),
                           void **(*next_getter)(void *)) {
    void *element = list.first;
    char found = 0;
    while (element && !found) {
        if (*id_getter(element) == id) {
            found = 1;
        } else {
            element = *next_getter(element);
        }
    }

    return element;
}

/* Universal element deletion */
void delete_list_element(ListHead *list, void *element,
                         void **(*next_getter)(void *),
                         void (*free_func)(void **)) {
    void *iter, *prev;
    iter = list->first;
    prev = NULL;
    while (iter) {
        if (iter == element) {
            if (iter == list->first) {
                list->first = *next_getter(iter);
            }
            if (iter == list->last) {
                list->last = prev;
            }
            if (prev) {
                *next_getter(prev) = *next_getter(iter);
            }
            free_func(&iter);
            list->count--;
        } else {
            prev = iter;
            iter = *next_getter(iter);
        }
    }
}

#endif